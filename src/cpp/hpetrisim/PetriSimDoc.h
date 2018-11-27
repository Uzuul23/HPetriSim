/**************************************************************************
	PetriSimDoc.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

#include "HDrawObject.h"
#include "HSimulation.h"
#include "HEditorEvent.h"
#include "OutputWnd.h"

class CPetriSimDoc;
class CHThread;
class CPetriSimView;
class CMainFrame;
class CHLineBuffer;
class CHNetMember;

class CPetriSimDoc : public CDocument
{

public:
	DECLARE_DYNCREATE(CPetriSimDoc)

	CPetriSimDoc();
	virtual ~CPetriSimDoc();

	void Subscribe(CHEditorEvent* p);
	void UnSubscribe(CHEditorEvent* p);
	void OnEditorEvent(CHEditorEvent::EditorEvent event);

	COutputWndTab* OutputWnd() const { return m_pOutputWnd; }
	void OutputWnd(COutputWndTab* val) { m_pOutputWnd = val; }
	
	CHSimulation& Sim() { return m_Sim; }

	//CDocument
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void OnChangedViewList();

private:
	
	bool m_bEnableOutputFile;
	bool m_bAlignNo;
	bool m_bShowGrid;
	bool m_bShowLabel;
	bool m_ConvertVersion;
	long m_Grid;
	
	CHLineBuffer* m_pOutputBuffer;
	CHDrawObject* object;
	CSize m_sizeDoc;
	CString m_csOutputFilePath;
	CHDrawObject* m_ExplObject;
	CHDrawObjectMap m_mapStoredObjects;
	CHDrawObjectList m_listObjects;
	CMap<CHDrawObject*, CHDrawObject*, POSITION, POSITION&> m_mapMembers;

//new
	GUID m_ClipboardId;
	LONG m_PasteCount;
	ULONG32 m_NextId;
	ULONG32 m_NextPositionIdent;
	ULONG32 m_NextTransitionIdent;
	ULONG32 m_NextConnectorIdent;
	bool m_bAlign;
	Color m_PaperColor;
	CMap<ULONG32, ULONG32, CString, CString&> m_mapConvertOldLabel;
	CHSimulation m_Sim;
	CArray<CHEditorEvent*> m_EventReceiver;
	COutputWndTab* m_pOutputWnd;

private:
	//int m_nAnimCurSteps;
	bool HandleInitError(const LPCTSTR cause = 0);
	CMainFrame& GetMainFrame();

public:

	//new
	enum UpdateHints 
	{ 
		UpdateDeleteContent = 1, 
		UpdateInvalidate,
		UpdateDocSize
	};
	
	bool GetShowLabel();
	int GetGrid();
	bool ShowGrid();
	ULONG32 GetNextId();
	ULONG32 NextPositionName() { return ++m_NextPositionIdent; }
	ULONG32 NextTransitionName() { return ++m_NextTransitionIdent; }
	ULONG32 NextConnectorName() { return ++m_NextConnectorIdent; }
	GUID ClipboardId() const { return m_ClipboardId; }
	void ClipboardId(GUID val) { m_ClipboardId = val; }
	LONG PasteCount() const { return m_PasteCount; }
	void PasteCount(LONG val) { m_PasteCount = val; }

	//old
	bool m_bRunFast;
	int m_nAnimPhase;
	UINT m_nIDEvent;
	bool AnimTokens();
	bool IfRunFast(){return m_bRunFast;};
	bool PeakAndPump();
	void SetEnabledOutputFile(bool enabled){m_bEnableOutputFile = enabled;};
	bool IsEnabledOutputFile(){return m_bEnableOutputFile;};
	CString GetOutputFilePath(){return m_csOutputFilePath;};
	void StopSim();
	void KillSimThread();
	void SetAlignDo(bool set){m_bAlign = set;};
	bool GetAlignDo(){return m_bAlign;};
	void SetAlignNo(bool set){m_bAlignNo = set;};
	bool GetAlignNo(){return m_bAlignNo;};
	void Draw(CDC& DC, bool grid);
	
	
	bool SimOnline();
	void PushBack();
//	void PopUp();
	LOGFONT m_lfDefaultFont;
	void ToGrid();
	CSize GetDocSize() { return m_sizeDoc; }

	//new
	void UpdateUI();
	void SimThreadMessage(WPARAM wParam, LPARAM lParam);
	void ToGrid(CPoint& point);
	void AddObjects(const CHDrawObjectList& list, CView* pView = 0);
	void RemoveObjects(const CHDrawObjectList& list);
	Color GetPaperColor();
	void AddToConvertOldLabelMap(ULONG32 id, CString & name);
	CHDrawObject* ObjectHit(const CPoint& point);
	CHNetMember* NetMemberHit( const CPoint& point );
	bool InitializeNet(CHDrawObjectList & list, bool OldFormat = false);
	void StrippNet(CHDrawObjectList& list);
	void UniqueName(const CHDrawObjectList& list);
	bool CanConnect(CHConnector* pConnector, CHNetMember* pNetMember, int Handle);
	CHDrawObjectList& DrawObjectList(){ return m_listObjects; }
	CHSimulation& Simulation() { return m_Sim; }
	

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	
protected:
	//afx_msg void OnUpdateMode(CCmdUI* pCmdUI);
	//afx_msg void OnMode(UINT nID);
	/*afx_msg void OnPositionMode();
	afx_msg void OnUpdatePositionMode(CCmdUI* pCmdUI);
	afx_msg void OnRectMode();
	afx_msg void OnUpdateRectMode(CCmdUI* pCmdUI);
	afx_msg void OnSelectMode();
	afx_msg void OnUpdateSelectMode(CCmdUI* pCmdUI);
	afx_msg void OnTransitionMode();
	afx_msg void OnUpdateTransitionMode(CCmdUI* pCmdUI);
	afx_msg void OnLineMode();
	afx_msg void OnUpdateLineMode(CCmdUI* pCmdUI);
	afx_msg void OnConnectorMode();
	afx_msg void OnUpdateConnectorMode(CCmdUI* pCmdUI);
	afx_msg void OnTextMode();
	afx_msg void OnUpdateTextMode(CCmdUI* pCmdUI);*/
	afx_msg void OnStepMode();
	afx_msg void OnUpdateStepMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUpdatePropertys(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePropertys();
	afx_msg void OnObjectPropertys();
	afx_msg void OnEditorProperties();
	afx_msg void OnEditorShowlabel();
	afx_msg void OnUpdateEditorShowlabel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorsStepTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorsSimTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorsSimStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorsCount(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSliderSimspeed(CCmdUI* pCmdUI);
	afx_msg void OnSymReset();
	afx_msg void OnSymRunfast();
	afx_msg void OnUpdateSymRunfast(CCmdUI* pCmdUI);
	afx_msg void OnSymRunnormal();
	afx_msg void OnUpdateSymRunnormal(CCmdUI* pCmdUI);
	afx_msg void OnSymSinglestep();
	afx_msg void OnUpdateSymSinglestep(CCmdUI* pCmdUI);
	afx_msg void OnSymStop();
	afx_msg void OnUpdateSymStop(CCmdUI* pCmdUI);
	afx_msg void OnPauseMode();
	afx_msg void OnUpdatePauseMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSymReset(CCmdUI* pCmdUI);
	afx_msg void OnSymDecreasespeed();
	afx_msg void OnUpdateSymDecreasespeed(CCmdUI* pCmdUI);
	afx_msg void OnSymIncreasespeed();
	afx_msg void OnUpdateSymIncreasespeed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorsStepCount(CCmdUI* pCmdUI);
	afx_msg void OnEnableWriteoutputfile();
	afx_msg void OnUpdateEnableWriteoutputfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditorProperties(CCmdUI* pCmdUI);
	afx_msg void OnFileExportNet();
	afx_msg void OnFileExportInterchange();
	afx_msg void OnFileImportInterchange();

	//new
	afx_msg void OnAlignGrid();
	afx_msg void OnUpdateAlignGrid(CCmdUI* pCmdUI);
	afx_msg void OnShowGrid();
	afx_msg void OnUpdateShowGrid(CCmdUI* pCmdUI);
	afx_msg void OnGridSize();
	afx_msg void OnUpdateGridSize(CCmdUI* pCmdUI);
	afx_msg void OnDocumentHeight();
	afx_msg void OnDocumentWeidth();
	afx_msg void OnDocumentColor();
	afx_msg void OnUpdateSimCommand(CCmdUI* pCmdUI);
	afx_msg void OnSimCommand(UINT nID);
	afx_msg void OnUpdateShowLabel(CCmdUI* pCmdUI);
	afx_msg void OnShowLabel();
	afx_msg void OnSimSpeed();

	DECLARE_MESSAGE_MAP()

};


inline bool CPetriSimDoc::GetShowLabel()
{
	return m_bShowLabel;
}

inline int CPetriSimDoc::GetGrid()
{
	return m_Grid;
}

inline bool CPetriSimDoc::ShowGrid()
{
	return m_bShowGrid;
}

inline Color CPetriSimDoc::GetPaperColor()
{
	return m_PaperColor;
}