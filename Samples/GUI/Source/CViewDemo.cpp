#include "pch.h"
#include "CViewDemo.h"

#include "imgui.h"
#include "CImguiManager.h"

#include "Context/CContext.h"
#include "Graphics2D/CGUIImporter.h"

CViewDemo::CViewDemo() :
	m_uiContainer(NULL),
	m_listTab(NULL),
	m_listItems(NULL),
	m_canvas(NULL)
{

}

CViewDemo::~CViewDemo()
{

}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	CScene* scene = context->getScene();
	scene->updateIndexSearchObject();

	CZone* zone = scene->getZone(0);

	CGameObject* leftPanel = zone->createEmptyObject();
	m_canvas = leftPanel->addComponent<CCanvas>();

	CGUIImporter::loadGUI("SampleGUIDemo/Main.gui", m_canvas);
	m_canvas->applyScaleGUI(1.0f);
	m_canvas->setSortDepth(0);

	m_uiContainer = leftPanel->addComponent<UI::CUIContainer>();

	m_txtTitle = dynamic_cast<CGUIText*>(m_canvas->getGUIByPath("Canvas/Container/Items/txtTabName"));

	// LIST: ITEMS
	m_listItems = new UI::CUIGridView(m_uiContainer,
		m_canvas->getGUIByPath("Canvas/Container/Items/ListItems"),
		m_canvas->getGUIByPath("Canvas/Container/Items/ListItems/Item"));

	CTextureManager* textureMgr = CTextureManager::getInstance();
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-0.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-1.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-2.png"));
	m_itemImage.push_back(textureMgr->getTexture("SampleGUIDemo/Demo/3d-cube-3.png"));

	// CATEGORY: TAB
	m_listTab = new UI::CUIListView(m_uiContainer,
		m_canvas->getGUIByPath("Canvas/Container/ListTab"),
		m_canvas->getGUIByPath("Canvas/Container/ListTab/Item"));

	CSpriteFrame* spriteFrame = CSpriteManager::getInstance()->loadSprite("SampleGUIDemo/TabIcon.spritedata");
	if (spriteFrame)
	{
		addTabItem(spriteFrame->getFrameByName("ic-hair"), "Hair");
		addTabItem(spriteFrame->getFrameByName("ic-shirt"), "Shirt");
		addTabItem(spriteFrame->getFrameByName("ic-pant"), "Pant");
		addTabItem(spriteFrame->getFrameByName("ic-heels"), "Heels");
	}
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onData()
{
	onChangeTab();
}

void CViewDemo::addTabItem(SFrame* frame, const char* name)
{
	if (frame)
	{
		CGUIElement* element = m_listTab->addItem();
		CGUISprite* iconGUI = (CGUISprite*)m_canvas->getGUIByPath(element, "Icon");
		if (iconGUI)
			iconGUI->setFrame(frame);

		CGUIElement* bg = m_canvas->getGUIByPath(element, "Background");
		if (bg)
		{
			UI::CUIButton* button = new UI::CUIButton(m_uiContainer, element);
			button->addMotion(UI::EMotionEvent::PointerHover, bg, new UI::CColorMotion(SColor(0xfffe7200)))->setTime(0.0f, 50.0f);
			button->addMotion(UI::EMotionEvent::PointerOut, bg, new UI::CColorMotion())->setTime(0.0f, 50.0f);
			button->setToggleButton(true);
			button->setSkipPointerEventWhenDrag(true);
			button->OnToggle = [bg, view = this, name](UI::CUIBase* button, bool status)
				{
					if (status)
					{
						bg->setColor(SColor(0xff0072fe));
						button->setEnable(false);

						view->onSelectTab(button, name);
					}
					else
					{
						bg->setColor(SColor(0xff201a2b));
					}
				};

			if (m_listUniformBtn.size() == 0)
				button->setToggle(true);

			m_listUniformBtn.push_back(button);
		}
	}
}

void CViewDemo::onSelectTab(UI::CUIBase* btn, const char* name)
{
	if (m_txtTitle)
		m_txtTitle->setText(name);

	m_tab = name;

	for (UI::CUIButton* b : m_listUniformBtn)
	{
		if (b != btn)
		{
			b->setToggle(false);
			b->setEnable(true);
		}
	}

	onChangeTab();
}

void CViewDemo::onChangeTab()
{
	m_listItems->clear();
	m_listItemsBtn.clear();

	if (m_tab == "Hair")
		addRandomItem(6);
	else if (m_tab == "Shirt")
		addRandomItem(4);
	else if (m_tab == "Pant")
		addRandomItem(3);
	else if (m_tab == "Heels")
		addRandomItem(9);
}

void CViewDemo::addRandomItem(int count)
{
	char name[128];

	for (int i = 0; i < count; i++)
	{
		CGUIElement* item = m_listItems->addItem();
		CGUIText* text = dynamic_cast<CGUIText*>(item->getGUIByPath("Text"));
		if (text)
		{
			sprintf(name, "Item %0d", i);
			text->setText(name);
		}

		CGUIImage* image = dynamic_cast<CGUIImage*>(item->getGUIByPath("Image"));
		if (image && m_itemImage.size() > 0)
		{
			s32 randomImg = getIrrlichtDevice()->getRandomizer()->rand() % (int)m_itemImage.size();
			image->setImage(m_itemImage[randomImg]);
		}

		CGUIElement* bg = item->getGUIByPath("Background");

		UI::CUIButton* button = new UI::CUIButton(m_uiContainer, item);
		button->addMotion(UI::EMotionEvent::PointerHover, bg, new UI::CColorMotion(SColor(0xfffe7200)))->setTime(0.0f, 50.0f);
		button->addMotion(UI::EMotionEvent::PointerOut, bg, new UI::CColorMotion())->setTime(0.0f, 50.0f);
		button->setToggleButton(true);
		button->setSkipPointerEventWhenDrag(true);
		button->OnToggle = [bg, view = this](UI::CUIBase* button, bool status)
			{
				if (status)
				{
					bg->setColor(SColor(0xff0072fe));
					button->setEnable(false);

					view->onSelectItem(button);
				}
				else
				{
					bg->setColor(SColor(0xff201a2b));
				}
			};
		m_listItemsBtn.push_back(button);
	}

	selectItem(m_listItemsBtn[0]);
}

void CViewDemo::selectItem(UI::CUIButton* btn)
{
	if (btn->getBackground())
		btn->getBackground()->setColor(SColor(0xff0072fe));
	btn->setEnable(false);

	onSelectItem(btn);
}

void CViewDemo::onSelectItem(UI::CUIBase* btn)
{
	for (UI::CUIButton* b : m_listItemsBtn)
	{
		if (b != btn)
		{
			b->setToggle(false);
			b->setEnable(true);
		}
	}
}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// imgui update
	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render all GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onGUI()
{

}

void CViewDemo::onPostRender()
{

}
