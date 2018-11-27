/**************************************************************************
	HSimulation.cpp

	copyright (c) 2013/08/24 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "PetriSimDoc.h"
#include "HUpdateObject.h"
#include "MainFrm.h"
#include "HSimulation.h"

UINT WINAPI Worker(LPVOID lpvParam)
{
	if (lpvParam == 0)
	{
		return 0;
	}

	return static_cast<CHSimulation*>(lpvParam)->Run();
}

CArray<CHSimulation*> CHSimulation::m_Simulations;
UINT_PTR CHSimulation::m_Timer = 0;

VOID CALLBACK TimerProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  UINT_PTR idEvent,_In_  DWORD dwTime)
{
	if (CHSimulation::m_Timer != idEvent)
	{
		return;
	}

	for (INT_PTR loop=0; loop<CHSimulation::m_Simulations.GetCount(); loop++)
	{
		CHSimulation::m_Simulations[loop]->OnTimer();
	}
}

void CHSimulation::AddTimer( CHSimulation* pSim )
{
	m_Simulations.Add(pSim);

	if (m_Simulations.GetCount() == 1)
	{
		m_Timer = ::SetTimer(0, 1, 50, TimerProc);
	}
}

void CHSimulation::RemoveTimer( CHSimulation* pSim )
{
	for (INT_PTR loop=0; loop<m_Simulations.GetCount(); loop++)
	{
		if (m_Simulations[loop] == pSim)
		{
			m_Simulations.RemoveAt(loop);
			break;
		}
	}

	if (m_Simulations.GetCount() == 0)
	{
		::KillTimer(0, m_Timer);
		m_Timer = 0;
	}
}

CHSimulation::CHSimulation(CPetriSimDoc* pDoc) : m_pDoc(pDoc)
, m_Mode(PauseMode)
, m_SimSpeed(1000)
, m_SimTime(0)
, m_SampleTime(1)
, m_hThread(0)
, m_ThreadId(-1)
, m_hWnd(0)
, m_AnimPhase(0)
, m_AnimSteps(0)
, m_AnimCurrentStep(0)
, m_TimeLimit(1000)
, m_StepLimit(1000)
, m_StepCount(0)
, m_Online(false)
{
	ASSERT_VALID(pDoc);

	CWnd* pWnd = AfxGetMainWnd();

	if (pWnd)
	{
		m_hWnd = pWnd->GetSafeHwnd();
	}
}

CHSimulation::~CHSimulation(void)
{
	if (m_Online)
	{
		Shutdown();
	}
}

void CHSimulation::OnUpdateSimCommand( CCmdUI* pCmdUI )
{
	switch (pCmdUI->m_nID)
	{
	case ID_SIM_RESET: pCmdUI->Enable(m_Mode == PauseMode); break;
	case ID_SIM_ON: pCmdUI->SetCheck(m_Online); break;
	case ID_SIM_PAUSE: pCmdUI->SetCheck(m_Mode == PauseMode); pCmdUI->Enable(m_Online); break;
	case ID_SIM_STEP: pCmdUI->SetCheck(m_Mode == StepMode); pCmdUI->Enable(m_Online); break;
	case ID_SIM_RUN: pCmdUI->SetCheck(m_Mode == RunMode); pCmdUI->Enable(m_Online); break;
	case ID_SIM_RUNFAST: pCmdUI->SetCheck(m_Mode == RunFastMode); pCmdUI->Enable(m_Online); break;
	case ID_SIM_TIME_LIMIT:
	case ID_SIM_STEP_LIMIT: pCmdUI->Enable(!m_Online); break;
	}
}

void CHSimulation::OnSimCommand( UINT nID )
{
	switch (nID)
	{
	case ID_SIM_RESET: if(m_Mode == PauseMode) Reset(); break;
	case ID_SIM_ON: m_Online ? Shutdown() : Online(); break;
	case ID_SIM_STEP:
		if (m_Online && m_Mode != StepMode)
		{
			m_Mode = StepMode;
			SetEvent(m_Events[1]);
		}
		break;
	case ID_SIM_RUN:
		if (m_Online && m_Mode != RunMode)
		{
			m_Mode = RunMode;
			SetEvent(m_Events[1]);
		}
		break;
	case ID_SIM_RUNFAST:
		if (m_Online && m_Mode != RunFastMode)
		{
			m_Mode = RunFastMode;
			SetEvent(m_Events[1]);
		}
		break;
	case ID_SIM_PAUSE:
		if (m_Mode == RunMode || m_Mode == RunFastMode)
		{
			m_Mode = PauseMode;
			SetEvent(m_Events[1]);
		}
		break;
	case ID_SIM_STEP_LIMIT:
		{
			CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

			if (pFrame)
			{
				CHPropertyValues vals;
				vals.Add(CHPropertyValue(static_cast<int>(ID_SIM_TIME_LIMIT)));

				pFrame->GetValueFromRibbon(vals[0]);
				m_StepLimit = vals[0];
			}		
		}
		break;
	case ID_SIM_TIME_LIMIT:
		{
			CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

			if (pFrame)
			{
				CHPropertyValues vals;
				vals.Add(CHPropertyValue(static_cast<int>(ID_SIM_TIME_LIMIT)));

				pFrame->GetValueFromRibbon(vals[0]);
				m_TimeLimit = vals[0];
			}
		}
		break;
	}

	SendMessage(StatusBar);
}

void CHSimulation::AddUpateUI(CHPropertyValues& vals)
{
	vals.Add(CHPropertyValue(m_StepLimit, ID_SIM_STEP_LIMIT));
	vals.Add(CHPropertyValue(m_TimeLimit, ID_SIM_TIME_LIMIT));

	SendMessage(StatusBar);
}

UINT CHSimulation::Run()
{

thread_start:;

	bool lock = false;

	while (true)
	{
		switch (WaitForMultipleObjects(2, m_Events, FALSE, INFINITE))
		{
		default:
		case WAIT_OBJECT_0: goto thread_exit;
		case WAIT_OBJECT_0+1:
			switch (m_Mode)
			{
			case StepMode:
				if(!SingleStep(false, lock))
				{
					goto thread_exit;
				}
				m_Mode = PauseMode;
				break;
			case RunMode:
				while (true)
				{
					DWORD Ret = ::WaitForSingleObject(m_Events[0], m_SimSpeed);

					if(Ret != WAIT_TIMEOUT)
					{
						//kill event
						m_Mode = PauseMode;
						goto thread_exit;
					}
					if(!SingleStep(false, lock))
					{
						//kill event
						m_Mode = PauseMode;
						goto thread_exit;
					}
					if (lock)
					{
						m_Mode = PauseMode;
						goto thread_start;
					}
					if (m_Mode != RunMode)
					{
						goto thread_start;
					}
				}
				break;
			case RunFastMode:
				while (true)
				{
					DWORD Ret = ::WaitForSingleObject(m_Events[0], 0);

					if(Ret != WAIT_TIMEOUT)
					{
						//kill event
						m_Mode = PauseMode;
						goto thread_exit;
					}

					if(!SingleStep(true, lock))
					{
						//kill event
						m_Mode = PauseMode;
						goto thread_exit;
					}
					if (lock)
					{
						m_Mode = PauseMode;
						goto thread_start;
					}
					if (m_Mode != RunFastMode)
					{
						SendMessage(RedrawNetObjects);
						goto thread_start;
					}
				}
				break;
			}
			break;
		}
	}

thread_exit:;
	SetEvent(m_Events[4]);
	return 0;
}

void CHSimulation::Reset()
{
	ASSERT_VALID(m_pDoc);

	m_SimTime = 0;
	m_StepCount = 0;

	CHDrawObjectList& list = m_pDoc->DrawObjectList();

	Rect rect;

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		Rect rect2 = pObject->GetBounds(true);

		if (pObject->Reset())
		{
			UnionRect(rect, rect2);
			UnionRect(rect, pObject->GetBounds(true));
		}
	}

	if (!rect.IsEmptyArea())
	{
		m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
	}
}

void CHSimulation::Online()
{
	ASSERT_VALID(m_pDoc);

	if (m_Online)
	{
		return;
	}

	m_Events[0] = CreateEvent(0, FALSE, FALSE, 0); //kill event
	m_Events[1] = CreateEvent(0, FALSE, FALSE, 0); //new order
	m_Events[2] = CreateEvent(0, FALSE, FALSE, 0); //new order accepted
	m_Events[3] = CreateEvent(0, FALSE, FALSE, 0); //token animation complete
	m_Events[4] = CreateEvent(0, FALSE, FALSE, 0); //thread ended

	CHDrawObjectList& list = m_pDoc->DrawObjectList();

	INT_PTR poscount = 0;
	INT_PTR trancount = 0;
	INT_PTR concount = 0;

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		if (pObject->IsKindOf(RUNTIME_CLASS(CHPosition)))
		{
			poscount++;
			continue;
		}

		if (pObject->IsKindOf(RUNTIME_CLASS(CHTransition)))
		{
			trancount++;
			continue;
		}

		if (pObject->IsKindOf(RUNTIME_CLASS(CHConnector)))
		{
			concount++;
			continue;
		}
	}

	m_Positions.SetSize(0, poscount);
	m_Transitions.SetSize(0, trancount);
	m_ConflictedTransitions.SetSize(0, trancount);
	m_ConnectorsAnimatedIn.SetSize(0, concount);
	m_ConnectorsAnimatedOut.SetSize(0, concount);

	pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		if (pObject->IsKindOf(RUNTIME_CLASS(CHPosition)))
		{
			m_Positions.Add(static_cast<CHPosition*>(pObject));
			continue;
		}

		if (pObject->IsKindOf(RUNTIME_CLASS(CHTransition)))
		{
			m_Transitions.Add(static_cast<CHTransition*>(pObject));
			continue;
		}
	}

	//scanning for potentially conflicts
	for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
	{
		bool conflict = false;

		CHTransition* pTran = m_Transitions[loop];

		Connectors& ins =  pTran->ConnectorsIn();

		for (INT_PTR loop2=0; loop2<ins.GetCount(); loop2++)
		{
			CHConnector* pCon = ins[loop2];

			if (!pCon->Connected())
			{
				continue;
			}

			if (pCon->From()->ConnectorsOut().GetCount() > 1)
			{
				pTran->Enabled(true);
				pTran->Invalid(true);
				conflict = true;
				break;
			}
		}

		if(!conflict)
		{
			Connectors& outs =  pTran->ConnectorsOut();

			for (INT_PTR loop2=0; loop2<outs.GetCount(); loop2++)
			{
				CHConnector* pCon = outs[loop2];

				if (!pCon->Connected())
				{
					continue;
				}

				if (pCon->From()->ConnectorsIn().GetCount() > 1)
				{
					pTran->Enabled(true);
					pTran->Invalid(true);
					conflict = true;
					break;
				}
			}
		}

		if (conflict)
		{
			m_ConflictedTransitions.Add(pTran);
		}
	}

	for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
	{
		CHTransition* pTran = m_Transitions[loop];

		pTran->Activ(false);
		pTran->Enabled(false);
	}

	CString strBuffer;

	m_strBuffer =  L"Petri Net Simulation V0.92\r\n";
	m_strBuffer += L"===========================\r\n";

	strBuffer.Format(L"Load: %d Position(s), %d Transition(s)\n\n", m_Positions.GetCount(), m_Transitions.GetCount());
	m_strBuffer += strBuffer;
	
	strBuffer.Format(L"Sample Time: %d ms\r\n", m_SampleTime);
	m_strBuffer += strBuffer;

	strBuffer.Format(L"Limits: %d ms or %d step(s)\r\n", m_TimeLimit, m_StepLimit);
	m_strBuffer += strBuffer;
	
	if(!m_ConflictedTransitions.IsEmpty())
	{
		strBuffer.Format(L"Warning: %d potentially Conflicts found\r\n", m_ConflictedTransitions.GetCount());
		m_strBuffer += strBuffer;

		for (INT_PTR loop=0; loop<m_ConflictedTransitions.GetCount(); loop++)
		{
			CHTransition* pTran = m_ConflictedTransitions[loop];

			strBuffer.Format(L"Transition: %s\r\n", pTran->Name());
			m_strBuffer += strBuffer;
		}
	}

	COutputWndTab* pWnd = m_pDoc->OutputWnd();

	if (pWnd)
	{
		pWnd->Show();
		pWnd->SetText(m_strBuffer);
	}
	

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Worker, (LPVOID)this, 0, &m_ThreadId); 

	if (m_hThread != 0)
	{
		m_Online = true;
		m_pDoc->OnEditorEvent(CHEditorEvent::EventSimOnline);
	}

	m_pDoc->UpdateAllViews(0);
}

void CHSimulation::Shutdown()
{
	if (!m_Online)
	{
		return;
	}

	RemoveTimer(this);

	m_AnimPhase = 0;

	SetEvent(m_Events[0]);
	if(WaitForSingleObject(m_Events[4], 10000) == WAIT_TIMEOUT)
	{
		ASSERT(FALSE);
		TerminateThread(m_hThread, -1);
	}

	INT_PTR events = sizeof(m_Events) / sizeof(HANDLE);

	for (INT_PTR loop=0; loop<events; loop++)
	{
		CloseHandle(m_Events[loop]);
	}

	m_hThread = 0;
	m_ThreadId = -1;

	m_Online = false;
	m_pDoc->OnEditorEvent(CHEditorEvent::EventSimShutdown);

	m_pDoc->UpdateAllViews(0);
}

void CHSimulation::Serialize( CArchive& ar )
{
	static const long version = 1;

	if (ar.IsStoring())
	{
		ar << version;
		ar << m_SimSpeed;
		ar << m_SimTime;
		ar << m_SampleTime;
		ar << m_TimeLimit;
		ar << m_StepLimit;
		ar << m_StepCount;
	}
	else
	{
		long version;

		ar >> version;
		ar >> m_SimSpeed;
		ar >> m_SimTime;
		ar >> m_SampleTime;
		ar >> m_TimeLimit;
		ar >> m_StepLimit;
		ar >> m_StepCount;
	}
}

bool CHSimulation::SingleStep(bool fast, bool& lock)
{
	bool bRet = true;
	lock = false;
	
	m_StepCount++;

	CString strBuffer;

	strBuffer.Format(L"Step: %d / Time: %d =====>\r\n", m_StepCount, m_SimTime);
	m_strBuffer = strBuffer;

	bool bTimeless = false;
	bool bDeadlock = true;
	// test switch ability of all transitions
	for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
	{
		CHTransition* pTran = m_Transitions[loop];

		//first deactivate
		bool bResetAktiv = pTran->Activ();
		pTran->Activ(false);

		bool bReset = pTran->Enabled();
		pTran->Enabled(false);

		Connectors& ins = pTran->ConnectorsIn();

		for (INT_PTR loop2=0; loop2<ins.GetCount(); loop2++)
		{
			CHConnector* pCon = ins[loop2];

			if (!pCon->Connected())
			{
				continue;
			}

			if(pCon->Type() == CHConnector::TypeInhibitor)
			{
				if(pCon->FromPosition()->GetToken() >= pCon->Weight())
				{
					goto nextTransition;
				}
			}
			else //CHConnector::TypeNormal
			{
				if(pCon->FromPosition()->GetToken() < pCon->Weight())
				{
					goto nextTransition;
				}
			}
		}

		Connectors& outs = pTran->ConnectorsOut();

		for (INT_PTR loop2=0; loop2<outs.GetCount(); loop2++)
		{
			CHConnector* pCon = outs[loop2];

			if (!pCon->Connected())
			{
				continue;
			}

			if(pCon->Type() == CHConnector::TypeInhibitor)
			{
				CHPosition* pPos = pCon->ToPosition();

				if((pPos->GetToken() - pPos->GetTokenMax()) >= pCon->Weight())
				{
					goto nextTransition;
				}
			}
			else //CHConnector::TypeNormal,TypeTest
			{
				CHPosition* pPos = pCon->ToPosition();

				if((pPos->GetTokenMax() - pPos->GetToken()) < pCon->Weight())
				{
					goto nextTransition;
				}
			}
		}

		if(pTran->GetDelay() > 0)
		{
			pTran->Enabled(true);
			pTran->Invalid(true);

			strBuffer.Format(L"Start Timer: %s\r\n", pTran->Name());
			m_strBuffer += strBuffer;

		}
		else
		{
			pTran->Activ(true);
			pTran->Enabled(false);
			pTran->Invalid(true);

			strBuffer.Format(L"Enable: %s\r\n", pTran->Name());
			m_strBuffer += strBuffer;
		}

nextTransition:;

		if(bResetAktiv || bReset)
		{
			//deactivate transition
			pTran->Invalid(true);
		}
		if(bReset && !pTran->Enabled() && (pTran->TimeMode() == CHTransition::TTM_DELAY))
		{
			pTran->SetDelay(pTran->GetStartDelay());
		}

	}

	//conflict detection
	for (INT_PTR loop=0; loop<m_ConflictedTransitions.GetCount(); loop++)
	{
		CHTransition* pTran = m_ConflictedTransitions[loop];

		if(pTran->Activ() && pTran->Invalid())
		{
			//branch conflict
			int ConCount = 0;

			Connectors& ins = pTran->ConnectorsIn();

			for (INT_PTR loop2=0; loop2<ins.GetCount(); loop2++)
			{
				CHConnector* pCon = ins[loop2];

				if (!pCon->Connected())
				{
					continue;
				}

				Connectors& outsa = pCon->From()->ConnectorsOut();

				if(outsa.GetCount() > 1)
				{
					ConCount = 0;

					for (INT_PTR loop3=0; loop3<outsa.GetCount(); loop3++)
					{
						CHConnector* pCon2 = outsa[loop3];

						if (!pCon2->Connected())
						{
							continue;
						}

						if(pCon2->Type() == CHConnector::TypeNormal)
						{
							CHTransition* pTran2 = pCon2->ToTransition();
							if(pTran2->Activ())
							{
								ConCount++;
								//pTran->Activ(false);
							}
						}
					}

					if(ConCount > 1)
					{
						m_strBuffer += L"Warning: branch conflict !\r\n";

						for (INT_PTR loop3=0; loop3<outsa.GetCount(); loop3++)
						{
							CHConnector* pCon2 = outsa[loop3];

							if(pCon2->Type() == CHConnector::TypeNormal)
							{
								CHTransition* pTran2 = pCon2->ToTransition();
								if(pTran2->Activ())
								{
									strBuffer.Format(L"Transition: %s\r\n", pTran2->Name());
									m_strBuffer += strBuffer;
								}
							}
						}

						ConCount = RandLong(0, ConCount);

						for (INT_PTR loop3=0; loop3<outsa.GetCount(); loop3++)
						{
							CHConnector* pCon2 = outsa[loop3];

							CHTransition* pTran2 = pCon2->ToTransition();

							pTran2->Invalid(false);

							if(pTran2->Activ())
							{
								if(loop3 != ConCount)
								{
									pTran2->Activ(false);

									strBuffer.Format(L"Disable Transition: %s\r\n", pTran2->Name());
									m_strBuffer += strBuffer;
								}
							}
						}
					}
				}
			}
			
			//meet conflict
			ConCount = 0;

			Connectors& outs = pTran->ConnectorsOut();

			for (INT_PTR loop2=0; loop2<outs.GetCount(); loop2++)
			{
				CHConnector* pCon = outs[loop2];

				if (!pCon->Connected())
				{
					continue;
				}

				Connectors& insa = pCon->To()->ConnectorsIn();

				if(insa.GetCount() > 1)
				{
					ConCount = 0;

					for (INT_PTR loop3=0; loop3<insa.GetCount(); loop3++)
					{
						CHConnector* pCon2 = insa[loop3];

						if (!pCon2->Connected())
						{
							continue;
						}

						if(pCon2->Type() == CHConnector::TypeNormal)
						{
							CHTransition* pTran2 = pCon2->FromTransition();

							if(pTran2->Activ())
							{
								ConCount++;
								//pTran->Activ(false);
							}
						}
					}
					if(ConCount > 1)
					{
						m_strBuffer += L"Warning: meet conflict !\r\n";

						for (INT_PTR loop3=0; loop3<insa.GetCount(); loop3++)
						{
							CHConnector* pCon2 = insa[loop3];

							if(pCon2->Type() == CHConnector::TypeNormal)
							{
								CHTransition* pTran2 = pCon2->FromTransition();
								if(pTran2->Activ())
								{
									strBuffer.Format(L"Transition: %s\r\n", pTran2->Name());
									m_strBuffer += strBuffer;
								}
							}
						}

						ConCount = RandLong(0, ConCount);

						for (INT_PTR loop3=0; loop3<insa.GetCount(); loop3++)
						{
							CHConnector* pCon2 = insa[loop3];

							CHTransition* pTran2 = pCon2->FromTransition();

							pTran2->Invalid(false);

							if(pTran2->Activ())
							{
								if(loop3 != ConCount)
								{
									pTran2->Activ(false);

									strBuffer.Format(L"Disable Transition: %s\r\n", pTran2->Name());
									m_strBuffer += strBuffer;
								}
							}
						}
					}
				}
			}
		}
	}

	//Transition input switching
	//create animation list
	for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
	{
		CHTransition* pTran = m_Transitions[loop];

		int fired = 0; //counter

		if(!pTran->Activ())
		{
			continue;
		}

		bDeadlock = false;

		Connectors& ins = pTran->ConnectorsIn();

		for (INT_PTR loop2=0; loop2<ins.GetCount(); loop2++)
		{
			CHConnector* pCon = ins[loop2];

			if (!pCon->Connected())
			{
				continue;
			}

			if (!fast)
			{
				m_ConnectorsAnimatedIn.Add(pCon);
			}

			CHPosition* pPos = pCon->FromPosition();

			if(pCon->Type() == CHConnector::TypeNormal)
			{
				strBuffer.Format(L"Position: %s decrement %d token(s)\r\n", pPos->Name(), pCon->Weight());
				m_strBuffer += strBuffer;

				pPos->SetToken(pPos->GetToken() - pCon->Weight());
				pPos->Invalid(true);
				fired += pCon->Weight();
				bTimeless = true;
			}
		}

		pTran->SetTokensCount(pTran->GetTokensCount() + fired);

		if (!fast)
		{
			Connectors& outs = pTran->ConnectorsOut();
			
			for (INT_PTR loop2=0; loop2<outs.GetCount(); loop2++)
			{
				m_ConnectorsAnimatedOut.Add(outs[loop2]);
			}
		}
	}

	//token animation
	if(!fast && m_SimSpeed >= 500)
	{
		if (m_ConnectorsAnimatedIn.GetCount() > 0 || m_ConnectorsAnimatedOut.GetCount() > 0)
		{
			SendMessage(RedrawNetObjects);
			SendMessage(TokenAnimation);

			HANDLE h[2] = { m_Events[0], m_Events[3] };
			if(WaitForMultipleObjects(2, h, FALSE, INFINITE) == WAIT_OBJECT_0)
			{
				return false;
			}
		}
	}

	//Transition output switching
	for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
	{
		CHTransition* pTran = m_Transitions[loop];

		int fired = 0; //counter

		if (!pTran->Activ())
		{
			continue;
		}

		bDeadlock = false;
		//restart delay
		switch(pTran->TimeMode())
		{
		case CHTransition::TTM_IMMIDIATE:break;
		case CHTransition::TTM_DELAY:
			pTran->SetDelay(pTran->GetStartDelay());
			break;
		case CHTransition::TTM_EXPONENTIAL:
			pTran->SetDelay(ExpRandLong(pTran->GetStartDelay()));
			break;
		case CHTransition::TTM_EQUAL_DISTR:
			pTran->SetDelay(RandLong(pTran->GetStartDelay(), pTran->GetRangeDelay()));
			break;
		}

		Connectors& outs = pTran->ConnectorsOut();

		for (INT_PTR loop2=0; loop2<outs.GetCount(); loop2++)
		{
			CHConnector* pCon = outs[loop2];

			CHPosition* pPosi = pCon->ToPosition();

			if(pCon->Type() == CHConnector::TypeNormal)
			{
				strBuffer.Format(L"Position: %s increment %d token(s)\r\n", pPosi->Name(), pCon->Weight());
				m_strBuffer += strBuffer;

				pPosi->SetToken(pPosi->GetToken() + pCon->Weight());
				pPosi->SetTokenCount(pPosi->GetTokenCount() + pCon->Weight());
				pPosi->Invalid(true);
				bTimeless = true;
			}
		}
	}

	if(!bTimeless)
	{
		for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
		{
			CHTransition* pTran = m_Transitions[loop];

			if((pTran->TimeMode() != CHTransition::TTM_IMMIDIATE) && pTran->Enabled())
			{
				bDeadlock = false;
				pTran->SetDelay(pTran->GetDelay() - m_SampleTime);
			}
		}

		if(!bDeadlock)
		{
			m_SimTime += m_SampleTime;
		}
	}

	if (m_SimTime > m_TimeLimit)
	{
		strBuffer.Format(L"Error: Time limit reached: %d ms, Step: %d\r\n", m_SimTime, m_StepCount);
		m_strBuffer += strBuffer;
		lock = true;

		if (fast)
		{
			m_strBuffer = strBuffer;
			SendMessage(RedrawNetObjects);
			SendMessage(Output);
		}
	}

	if (m_StepCount > m_StepLimit)
	{
		strBuffer.Format(L"Error: Step limit reached: %d ms, Step: %d\r\n", m_SimTime, m_StepCount);
		m_strBuffer += strBuffer;
		lock = true;

		if (fast)
		{
			m_strBuffer = strBuffer;
			SendMessage(RedrawNetObjects);
			SendMessage(Output);
		}
	}

	if(bDeadlock)
	{
		strBuffer.Format(L"Error: Deadlock at Time: %d ms, Step: %d\r\n", m_SimTime, m_StepCount);
		m_strBuffer += strBuffer;
		lock = true;

		if (fast)
		{
			m_strBuffer = strBuffer;
			SendMessage(RedrawNetObjects);
			SendMessage(Output);
		}
	}

//TODO:
//	if(m_pDoc->IsEnabledOutputFile())
//		WriteStepToFile();

	if (!fast)
	{
		SendMessage(RedrawNetObjects);
		SendMessage(Output);
	}
	SendMessage(StatusBar);

	return bRet;
}

void CHSimulation::OnSimThreadMessage( WPARAM wParam, LPARAM lParam )
{
	switch (static_cast<UpdateMode>(wParam))
	{
	case Output:
		{
			COutputWndTab* pWnd = m_pDoc->OutputWnd();
			if (pWnd)
			{
				pWnd->SetText(m_strBuffer);
			}
		}
		break;
	case RedrawNetObjects:
		{
			Rect rect;

			for (INT_PTR loop=0; loop<m_Transitions.GetCount(); loop++)
			{
				CHTransition* pTran = m_Transitions[loop];

				if (!pTran->Invalid())
				{
					continue;
				}

				UnionRect(rect, pTran->GetBounds(true));

				pTran->Invalid(false);
			}

			for (INT_PTR loop=0; loop<m_Positions.GetCount(); loop++)
			{
				CHPosition* pPos = m_Positions[loop];

				if (!pPos->Invalid())
				{
					continue;
				}

				UnionRect(rect, pPos->GetBounds(true));

				pPos->Invalid(false);
			}

			if (!rect.IsEmptyArea())
			{
				m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
			}
		}
		break;
	case TokenAnimation:
		AnimateTokens();
		break;
	case StatusBar:
		{
			CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());

			if (pFrame)
			{
				CString str;
				if (m_Online)
				{
					str.Format(L"ONLINE S:%d T:%d ms", m_StepCount, m_SimTime);
				}
				else
				{
					str.Format(L"OFFLINE S:%d T:%d ms", m_StepCount, m_SimTime);
				}
				
				pFrame->UpdateStatusBar(ID_STATUSBAR_SIMSTATUS, str);
			}
		}
		break;
	}
}

void CHSimulation::DrawTokenAnimation(Graphics& g, UINT nHint /*= 0*/)
{
	switch (m_AnimPhase)
	{
	case 1:
		{
			SolidBrush brush(Color::DarkGray);

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedIn.GetCount(); loop++)
			{
				const Point token = m_ConnectorsAnimatedIn[loop]->TokenLocation();

				Rect rect(token, Size(0, 0));
				rect.Inflate(5, 5);

				g.FillEllipse(&brush, rect);
			}
		}
		break;
	case 3:
		{
			SolidBrush brush(Color::DarkGray);

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedOut.GetCount(); loop++)
			{
				const Point token = m_ConnectorsAnimatedOut[loop]->TokenLocation();

				Rect rect(token, Size(0, 0));
				rect.Inflate(5, 5);

				g.FillEllipse(&brush, rect);
			}
		}
		break;
	}
}

void CHSimulation::AnimateTokens()
{
	switch (m_AnimPhase)
	{
	case 0:
		{
			AddTimer(this);
			m_AnimPhase = 1;
			m_AnimSteps = m_SimSpeed / 100;
			m_AnimCurrentStep = 0;

			Rect rect;

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedIn.GetCount(); loop++)
			{
				m_ConnectorsAnimatedIn[loop]->StartTokenAnim();

				Rect rect2(m_ConnectorsAnimatedIn[loop]->TokenLocation(), Size(0, 0));
				rect2.Inflate(5, 5);

				UnionRect(rect, rect2);
			}

			m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case 1:
		m_AnimCurrentStep++;

		if (m_AnimCurrentStep > m_AnimSteps)
		{
			m_AnimPhase = 2;

			Rect rect;

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedIn.GetCount(); loop++)
			{
				Rect rect2(m_ConnectorsAnimatedIn[loop]->TokenLocation(), Size(0, 0));
				rect2.Inflate(5, 5);

				UnionRect(rect, rect2);
			}

			m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		else
		{
			Rect rect;

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedIn.GetCount(); loop++)
			{
				Rect rect2(m_ConnectorsAnimatedIn[loop]->TokenLocation(), Size(0, 0));
				rect2.Inflate(5, 5);

				UnionRect(rect, rect2);

				m_ConnectorsAnimatedIn[loop]->StepTokenAnim(m_AnimSteps);

				Rect rect3(m_ConnectorsAnimatedIn[loop]->TokenLocation(), Size(0, 0));
				rect3.Inflate(5, 5);

				UnionRect(rect, rect3);
			}

			m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case 2:
		{

			m_AnimPhase = 3;
			m_AnimSteps = m_SimSpeed / 100;
			m_AnimCurrentStep = 0;

			Rect rect;

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedOut.GetCount(); loop++)
			{
				m_ConnectorsAnimatedOut[loop]->StartTokenAnim();

				Rect rect2(m_ConnectorsAnimatedOut[loop]->TokenLocation(), Size(0, 0));
				rect2.Inflate(5, 5);

				UnionRect(rect, rect2);
			}

			m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case 3:
		m_AnimCurrentStep++;

		if (m_AnimCurrentStep > m_AnimSteps)
		{
			m_AnimPhase = 4;
		}
		else
		{
			Rect rect;

			for (INT_PTR loop=0; loop<m_ConnectorsAnimatedOut.GetCount(); loop++)
			{
				Rect rect2(m_ConnectorsAnimatedOut[loop]->TokenLocation(), Size(0, 0));
				rect2.Inflate(5, 5);

				UnionRect(rect, rect2);

				m_ConnectorsAnimatedOut[loop]->StepTokenAnim(m_AnimSteps);

				Rect rect3(m_ConnectorsAnimatedOut[loop]->TokenLocation(), Size(0, 0));
				rect3.Inflate(5, 5);

				UnionRect(rect, rect3);
			}	

			m_pDoc->UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case 4:
		m_AnimPhase = 0;
		
		RemoveTimer(this);
		
		m_ConnectorsAnimatedIn.RemoveAll();
		m_ConnectorsAnimatedOut.RemoveAll();

		m_pDoc->UpdateAllViews(0);

		SetEvent(m_Events[3]);
		break;
	}
}

void CHSimulation::OnTimer()
{
	AnimateTokens();
}

void CHSimulation::SendMessage( UpdateMode mode )
{
	ASSERT(m_hWnd && ::IsWindow(m_hWnd));

	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		::SendMessage(m_hWnd, WM_SIMTHREAD, static_cast<WPARAM>(mode), reinterpret_cast<LPARAM>(m_pDoc));
	}
}
