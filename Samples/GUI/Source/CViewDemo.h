#pragma once

#include "ViewManager/CView.h"
#include "GameObject/CGameObject.h"

#include "UserInterface/CUIContainer.h"
#include "UserInterface/CUIListView.h"
#include "UserInterface/CUIGridView.h"
#include "UserInterface/CUIButton.h"

class CViewDemo : public CView
{
protected:
	UI::CUIContainer* m_uiContainer;
	UI::CUIListView* m_listTab;
	UI::CUIGridView* m_listItems;

	CGUIText* m_txtTitle;

	std::vector<UI::CUIButton*> m_listUniformBtn;
	CCanvas* m_canvas;

	std::string m_tab;

	std::vector<ITexture*> m_itemImage;
	std::vector<UI::CUIButton*> m_listItemsBtn;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onData();

protected:

	void onGUI();

	void onChangeTab();

	void addRandomItem(int count);

	void addTabItem(SFrame* frame, const char* name);

	void onSelectTab(UI::CUIBase* btn, const char* name);

	void onSelectItem(UI::CUIBase* btn);

	void selectItem(UI::CUIButton* btn);
};
