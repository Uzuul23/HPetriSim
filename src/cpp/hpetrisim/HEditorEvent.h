/**************************************************************************
	EditorEvent.h

	copyright (c) 2013/08/31 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CHEditorEvent
{
public:
	enum EditorEvent
	{
		EventSimOnline,
		EventSimShutdown
	};
	virtual void OnEditorEvent(EditorEvent event) = 0;
};