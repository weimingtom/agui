/*   _____                           
 * /\  _  \                     __    
 * \ \ \_\ \      __    __  __ /\_\   
 *  \ \  __ \   /'_ `\ /\ \/\ \\/\ \  
 *   \ \ \/\ \ /\ \_\ \\ \ \_\ \\ \ \ 
 *    \ \_\ \_\\ \____ \\ \____/ \ \_\
 *     \/_/\/_/ \/____\ \\/___/   \/_/
 *                /\____/             
 *                \_/__/              
 *
 * Copyright (c) 2011 Joshua Larouche
 * 
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Agui nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Agui/Widgets/DropDown/DropDown.hpp"

namespace agui {
	DropDown::DropDown( ListBox *listbox /*= NULL*/ )
	: listBoxHeight(400),selIndex(-1),resizeToWidestItem(false),
	mouseInside(false)
	{
		if(listbox)
		{
			isMaintainingListBox = false;
			pChildListBox = listbox;
		}
		else
		{
			isMaintainingListBox = true;
			pChildListBox = new ListBox();
		}

		pChildListBox->addListBoxListener(this);
		setFocusable(true);
		setTabable(true);
		setupListBox();
	}

	DropDown::~DropDown(void)
	{
		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it))
				(*it)->death(this);
		}

		for(std::vector<Widget*>::iterator it = getPrivateChildBegin(); it != 
			getPrivateChildEnd(); ++it)
		{
			(*it)->removeMouseListener(this);
			(*it)->removeKeyboardListener(this);
		}
		for(std::vector<Widget*>::iterator it = getChildBegin(); it != 
			getChildEnd(); ++it)
		{
			(*it)->removeMouseListener(this);
			(*it)->removeKeyboardListener(this);
		}
		pChildListBox->removeListBoxListener(this);

		if(isMaintainingListBox)
		{
			delete pChildListBox;
		}
	}

	void DropDown::setMaxDropDownHeight(int height)
	{
		listBoxHeight = height;
		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it))
				(*it)->maxDropDownHeightChanged(this,height);
		}
	}

	int DropDown::getMaxDropDownHeight() const
	{
		return listBoxHeight;
	}

	void DropDown::setupListBox()
	{
		pChildListBox->setSize(getWidth(),getMaxDropDownHeight());
		pChildListBox->setMultiselect(false);
		pChildListBox->setMultiselectExtended(false);
		pChildListBox->setHoverSelection(true);
		pChildListBox->setFocusable(true);
		pChildListBox->hide();
		pChildListBox->setSelectedIndex(-1);
		pChildListBox->addMouseListener(this);
		pChildListBox->addKeyboardListener(this);
	}

	void DropDown::positionListBox()
	{
		pChildListBox->setLocation(getAbsolutePosition().getX() + getListPositionOffset().getX(),
			getAbsolutePosition().getY() + getHeight() + getListPositionOffset().getY());
	}

	void DropDown::setSize( const Dimension &size )
	{
		Widget::setSize(size);
		positionListBox();
	}

	void DropDown::setSize( int width, int height )
	{
		Widget::setSize(width,height);
	}

	void DropDown::setLocation( const Point &location )
	{
		Widget::setLocation(location);
		positionListBox();
	}

	void DropDown::setLocation( int width,int height )
	{
		Widget::setLocation(width,height);
	}


	void DropDown::showDropDown()
	{
		if(getTopWidget())
		{
			Widget* top = getTopWidget();
			if(pChildListBox->getParent())
			{
				pChildListBox->getParent()->remove(pChildListBox);
			}
			top->add(pChildListBox);
		}
		pChildListBox->show();
		positionListBox();
		resizeListBox();
		pChildListBox->setSelectedIndex(getSelectedIndex());
		if(getSelectedIndex() <= -1)
		{
			pChildListBox->moveToSelection(0);
		}
		else
		{
			pChildListBox->moveToSelection(pChildListBox->getLength() - 1);
			pChildListBox->moveToSelection(getSelectedIndex());
		}
		pChildListBox->requestModalFocus();

		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it))
				(*it)->dropDownShown(this);
		}
	}

	void DropDown::hideDropDown()
	{
		pChildListBox->hide();
		if(pChildListBox->getParent())
		{
			pChildListBox->getParent()->remove(pChildListBox);
		}
		pChildListBox->releaseModalFocus();
		focus();

		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it))
				(*it)->dropDownHidden(this);
		}
	}

	void DropDown::resizeListBox()
	{
		int sizeY = pChildListBox->getItemHeight();
		int sizeX = getWidth();
		int vscroll = 0;
		if(pChildListBox->isVScrollNeeded())
		{
			vscroll = pChildListBox->getVScrollSize().getWidth();
		}
		if(isResizingToWidestItem() &&
			sizeX < pChildListBox->getContentWidth() + vscroll)
		{
			sizeX = pChildListBox->getContentWidth() + vscroll +
				pChildListBox->getMargin(SIDE_LEFT) +
				pChildListBox->getMargin(SIDE_RIGHT) + 5;

		}
		if(pChildListBox->getLength() > 0)
		{
			pChildListBox->resizeHeightToContents();
			sizeY = pChildListBox->getHeight();
		}

		if(sizeY > getMaxDropDownHeight())
		{
			sizeY = getMaxDropDownHeight();
		}

		pChildListBox->setSize(
			sizeX + getListSizePadding().getWidth(),sizeY + getListSizePadding().getHeight());
	}

	void DropDown::modalMouseDownCB( MouseEvent &mouseEvent )
	{
		if(mouseEvent.getButton() == MOUSE_BUTTON_LEFT)
		{
			hideDropDown();
			mouseEvent.consume();
		}
	}

	void DropDown::mouseDown( MouseEvent &mouseEvent )
	{
		if(mouseEvent.getButton() == MOUSE_BUTTON_LEFT)
		{
			showDropDown();
			mouseEvent.consume();
		}
		
	}

	void DropDown::mouseClickCB( MouseEvent &mouseEvent )
	{
		if(mouseEvent.getButton() == MOUSE_BUTTON_LEFT)
		{
			hideDropDown();
			int val = getSelectedIndex();
			setSelectedIndex(pChildListBox->getSelectedIndex());
			if(val != getSelectedIndex())
			{
				dispatchActionEvent(ActionEvent(
					this));
			}

		}
		
	}

	void DropDown::paintBackground( const PaintEvent &paintEvent )
	{
		//draw background
		Color color = getBackColor();
		if(isFocused())
		{
			color = Color(200,220,230);
		}
		if(isDropDownShowing())
		{
			color = Color(160,180,190);
		}
		paintEvent.graphics()->drawFilledRectangle(getSizeRectangle(),color);

		Color  Top = Color(100,100,100);
		Color  Left = Color(100,100,100);
		Color  Bottom = Color(100,100,100);
		Color  Right = Color(100,100,100);


		//top
		paintEvent.graphics()->drawLine(Point(0,1),
			Point(getSize().getWidth(),1),Top);

		//left
		paintEvent.graphics()->drawLine(Point(1,1),
			Point(1,getSize().getHeight()),Left);

		//right
		paintEvent.graphics()->drawLine(Point(getSize().getWidth() ,1),
			Point(getSize().getWidth() ,getSize().getHeight()),Right);

		//bottom
		paintEvent.graphics()->drawLine(Point(0,getSize().getHeight()),
			Point(getSize().getWidth(),getSize().getHeight()),Bottom);
	}

	void DropDown::paintComponent( const PaintEvent &paintEvent )
	{
		int rem = getInnerHeight() - getFont()->getLineHeight();
		rem /= 2;

		paintEvent.graphics()->drawText(Point(0,rem),getText().c_str(),
			getFontColor(),getFont());

		int iters = 8;
		int xPos = getInnerWidth() - ( 2 * iters);
		xPos -= 2;
		int yPos = (getInnerHeight() - (iters / 2)) / 2;
		for(int i = iters; i > 0; --i)
		{
			paintEvent.graphics()->drawLine(
				Point((2 * iters) - i + xPos,i + yPos),
				Point(i + xPos,i + yPos),
				getFontColor());
		}
	}

	void DropDown::modalMouseUpCB( MouseEvent &mouseEvent )
	{
		if(mouseEvent.getButton() == MOUSE_BUTTON_LEFT)
		{
			hideDropDown();
			mouseEvent.consume();
		}
	}

	void DropDown::setSelectedIndex( int index )
	{
		pChildListBox->setSelectedIndex(index);
		selIndex = pChildListBox->getSelectedIndex();
		setText(pChildListBox->getItemAt(selIndex));
		dispatchSelectionEvent();
	}

	void DropDown::keyDownCB(KeyEvent &keyEvent )
	{
		if(keyEvent.getExtendedKey() == EXT_KEY_DOWN ||
			keyEvent.getExtendedKey() == EXT_KEY_UP ||
			keyEvent.getExtendedKey() == EXT_KEY_HOME ||
			keyEvent.getExtendedKey() == EXT_KEY_END ||
			keyEvent.getKey() == KEY_ENTER
			)
		{
			keyEvent.consume();
			setSelectedIndex(pChildListBox->getSelectedIndex());
		}

		if(keyEvent.getKey() == KEY_ENTER)
		{
			keyEvent.consume();
			hideDropDown();
		}
	}

	int DropDown::getSelectedIndex() const
	{
		return selIndex;
	}

	void DropDown::keyRepeatCB( KeyEvent &keyEvent )
	{
		if(keyEvent.getExtendedKey() == EXT_KEY_DOWN ||
			keyEvent.getExtendedKey() == EXT_KEY_UP ||
			keyEvent.getExtendedKey() == EXT_KEY_HOME ||
			keyEvent.getExtendedKey() == EXT_KEY_END ||
			keyEvent.getKey() == KEY_ENTER
			)
		{
			keyEvent.consume();
			setSelectedIndex(pChildListBox->getSelectedIndex());
		}

		if(keyEvent.getKey() == KEY_ENTER)
		{
			keyEvent.consume();
			hideDropDown();
		}
	}

	void DropDown::keyUpCB( KeyEvent &keyEvent )
	{

	}

	void DropDown::keyDown( KeyEvent &keyEvent )
	{
		int val = getSelectedIndex();
		handleKeyboard(keyEvent);
		if(val != getSelectedIndex())
		{
			dispatchActionEvent(ActionEvent(
				this));
		}
	}

	void DropDown::handleKeyboard( KeyEvent &keyEvent )
	{
		if(keyEvent.getKey() == KEY_SPACE)
		{
			showDropDown();
			keyEvent.consume();
			return;
		}
		if(keyEvent.getExtendedKey() == EXT_KEY_DOWN ||
			keyEvent.getExtendedKey() == EXT_KEY_RIGHT)
		{
			keyEvent.consume();
			if(pChildListBox->getSelectedIndex() != getSelectedIndex())
			{
				setSelectedIndex(pChildListBox->getSelectedIndex());
				return;
			}
			pChildListBox->setSelectedIndex(getSelectedIndex() + 1);
			setSelectedIndex(pChildListBox->getSelectedIndex());
		}
		else if(keyEvent.getExtendedKey() == EXT_KEY_UP ||
			keyEvent.getExtendedKey() == EXT_KEY_LEFT)
		{
			if(pChildListBox->getSelectedIndex() != getSelectedIndex())
			{
				keyEvent.consume();
				setSelectedIndex(pChildListBox->getSelectedIndex());
				return;
			}
			if(getSelectedIndex() <= 0)
			{
				return;
			}
			pChildListBox->setSelectedIndex(getSelectedIndex() - 1);
			setSelectedIndex(pChildListBox->getSelectedIndex());
			keyEvent.consume();
		}
	}

	void DropDown::keyRepeat( KeyEvent &keyEvent )
	{
		int val = getSelectedIndex();
		handleKeyboard(keyEvent);
		if(getSelectedIndex() != val)
		{
			dispatchActionEvent(ActionEvent(
				this));
		}
	}

	void DropDown::setResizeToWidestItem( bool resize )
	{
		resizeToWidestItem = resize;
		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it))
				(*it)->resizeToWidestItemChanged(this,resize);
		}
	}

	bool DropDown::isResizingToWidestItem() const
	{
		return resizeToWidestItem;
	}

	bool DropDown::isDropDownShowing() const
	{
		return pChildListBox->isVisible();
	}

	void DropDown::addItem( const std::string &item )
	{
		pChildListBox->addItem(item);
	}

	void DropDown::setFont( const Font *font )
	{
		Widget::setFont(font);
		if(getInnerHeight() < getFont()->getLineHeight())
		{
			setSize(getWidth(),getFont()->getLineHeight() 
				+ getMargin(SIDE_TOP) 
				+ getMargin(SIDE_BOTTOM));
		}
	}

	void DropDown::dispatchSelectionEvent()
	{
		for(std::vector<SelectionListener*>::iterator it = selectionListeners.begin();
			it != selectionListeners.end(); ++it)
		{
			(*it)->selectionChanged(this,getText(),getSelectedIndex(),true);
		}
	}

	void DropDown::addSelectionListener( SelectionListener* listener )
	{
		if(!listener)
		{
			return;
		}
		for(std::vector<SelectionListener*>::iterator it = 
			selectionListeners.begin();
			it != selectionListeners.end(); ++it)
		{
			if((*it) == listener)
				return;
		}

		selectionListeners.push_back(listener);
	}

	void DropDown::removeSelectionListener( SelectionListener* listener )
	{
		selectionListeners.erase(
			std::remove(selectionListeners.begin(),
			selectionListeners.end(), listener),
			selectionListeners.end());
	}

	void DropDown::removeItem( const std::string& item )
	{
		pChildListBox->removeItem(item);
	}

	void DropDown::removeItemAt( int index )
	{
		pChildListBox->removeItemAt(index);
	}

	void DropDown::addItemAt( const std::string& item, int index )
	{
		pChildListBox->addItemAt(item,index);
	}

	void DropDown::itemAdded( ListBox* source, const std::string& item )
	{
		if(source == pChildListBox)
		{
			for(std::vector<DropDownListener*>::iterator it = 
				dropDownListeners.begin();
				it != dropDownListeners.end(); ++it)
			{
				if((*it))
					(*it)->itemAdded(this,item);
			}
		}
	}

	void DropDown::itemRemoved( ListBox* source, const std::string& item )
	{
		if(source == pChildListBox)
		{
			for(std::vector<DropDownListener*>::iterator it = 
				dropDownListeners.begin();
				it != dropDownListeners.end(); ++it)
			{
				if((*it))
					(*it)->itemRemoved(this,item);
			}
		}
	}

	void DropDown::addDropDownListener( DropDownListener* listener )
	{
		if(!listener)
		{
			return;
		}
		for(std::vector<DropDownListener*>::iterator it = 
			dropDownListeners.begin();
			it != dropDownListeners.end(); ++it)
		{
			if((*it) == listener)
				return;
		}

		dropDownListeners.push_back(listener);
	}

	void DropDown::removeDropDownListener( DropDownListener* listener )
	{
		dropDownListeners.erase(
			std::remove(dropDownListeners.begin(),
			dropDownListeners.end(), listener),
			dropDownListeners.end());
	}

	const Point& DropDown::getListPositionOffset() const
	{
		return listPosOffset;
	}

	void DropDown::setListPositionOffset( const Point& offset )
	{
		listPosOffset = offset;
	}

	void DropDown::setListSizePadding( const Dimension& padding )
	{
		listSizeIncrease = padding;
	}

	const Dimension& DropDown::getListSizePadding() const
	{
		return listSizeIncrease;
	}

	void DropDown::mouseEnter( MouseEvent &mouseEvent )
	{
		agui::Widget::mouseEnter(mouseEvent);
		mouseInside = true;
	}

	void DropDown::mouseLeave( MouseEvent &mouseEvent )
	{
		agui::Widget::mouseLeave(mouseEvent);
		mouseInside = false;
	}

	bool DropDown::isMouseInside() const
	{
		return mouseInside;
	}


}
