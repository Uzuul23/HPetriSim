/**************************************************************************
	HSimulation.h

	copyright (c) 2013/08/24 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

#include "HTransition.h"
#include "HPosition.h"
#include "HConnector.h"
#include "HPropertyValue.h"
#include "HRandom.h"

class CPetriSimDoc;
class CHSimulation : CHRandom
{
public:
	CHSimulation(CPetriSimDoc* pDoc);
	virtual ~CHSimulation(void);

	void OnSimThreadMessage(WPARAM wParam, LPARAM lParam);
	void OnUpdateSimCommand( CCmdUI* pCmdUI );
	void OnSimCommand( UINT nId );
	void AddUpateUI(CHPropertyValues& vals);

	void Serialize(CArchive& ar);

	void Reset();
	void Online();
	bool IsOnline() { return m_Online; }
	void Shutdown();

	void DrawTokenAnimation(Graphics& g, UINT nHint = 0 );

	long SimSpeed() const { return m_SimSpeed; }
	void SimSpeed(long val) { m_SimSpeed = val; }

	long SimTime() const { return m_SimTime; }
	void SimTime(long val) { m_SimTime = val; }

	long SampleTime() const { return m_SampleTime; }
	void SampleTime(long val) { m_SampleTime = val; }

	enum SimMode
	{
		PauseMode,
		StepMode,
		RunMode,
		RunFastMode
	};

	enum UpdateMode
	{
		RedrawNetObjects,
		TokenAnimation,
		Output,
		StatusBar
	};

	UINT Run();

private:
	void SendMessage(UpdateMode mode);
	bool SingleStep(bool fast, bool& lock);
	void AnimateTokens();

	bool m_Online;
	CPetriSimDoc* m_pDoc;
	SimMode m_Mode;
	long m_SimSpeed;
	long m_SimTime;
	long m_SampleTime;
	long m_StepCount;
	HANDLE m_hThread;
	DWORD m_ThreadId;
	long m_AnimPhase;
	long m_AnimSteps;
	long m_AnimCurrentStep;
	long m_TimeLimit;
	long m_StepLimit;
	CString m_strBuffer;
	CArray<CHPosition*> m_Positions;
	CArray<CHTransition*> m_Transitions;
	CArray<CHTransition*> m_ConflictedTransitions;
	CArray<CHConnector*> m_ConnectorsAnimatedIn;
	CArray<CHConnector*> m_ConnectorsAnimatedOut;
	HANDLE  m_Events[5];
	HWND m_hWnd;

public:
	void OnTimer();
	static CArray<CHSimulation*> m_Simulations;
	static UINT_PTR m_Timer;
	static void AddTimer(CHSimulation* pSim);
	static void RemoveTimer(CHSimulation* pSim);
};
