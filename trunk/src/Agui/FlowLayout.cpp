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

#include "Agui/FlowLayout.hpp"
namespace agui
{

	FlowLayout::FlowLayout(void)
		:verticalSpacing(10),
		horizontalSpacing(10),
		topToBottom(true),leftToRight(true),
		singleRow(false), center(false),
		alignLastRow(false)
	{
	}

	FlowLayout::~FlowLayout(void)
	{
	}

	void FlowLayout::layoutChildren()
	{
		
			int curX = 0;
			int curY = 0;

			int highestWidget = 0;
			int numWidgets = 0;
			int rlOffset = 0;
			int btOffset = 0;
			int numRows = 1;

			std::vector<Widget*> curRow;
			Widget* firstWidget = NULL;
			for(std::list<Widget*>::iterator it = getChildBegin(); 
				it != getChildEnd(); ++it)
			{
				if(!(*it)->isVisible())
				{
					continue;
				}

				if(!firstWidget)
				{
					firstWidget = (*it);
				}

				if(curX + (*it)->getWidth() > getInnerWidth() && numWidgets > 0 && !singleRow)
				{
					numRows++;
					curX = 0;
					curY += highestWidget + getVerticalSpacing();
					highestWidget = 0;

					if(center && curRow.size() > 0)
					{
						int x1 = curRow[0]->getLocation().getX();
						int x2 = curRow.back()->getLocation().getX() +
							curRow.back()->getWidth();

						int w = x2 - x1;
						int centerOffset = (getInnerWidth() - w) / 2;

						for(size_t i = 0; i < curRow.size(); ++i)
						{
							curRow[i]->setLocation(
								curRow[i]->getLocation().getX() + centerOffset,
								curRow[i]->getLocation().getY());
						}
					}

					curRow.clear();
				}

				if(!topToBottom)
				{
					btOffset = getInnerHeight() - (*it)->getHeight() - (2 * curY);
				}
				if(!leftToRight && !center)
				{
					rlOffset = getInnerWidth() - (*it)->getWidth() - (2 * curX);
				}
				(*it)->setLocation(curX + rlOffset,curY + btOffset);
				curX += (*it)->getWidth() + getHorizontalSpacing();
				numWidgets++;

				if((*it)->getHeight() > highestWidget)
				{
					highestWidget = (*it)->getHeight();
				}

				curRow.push_back((*it));
		}

			//code duplication, I know :(
			if(center && curRow.size() > 0)
			{
				if(alignLastRow && numRows > 1 && firstWidget)
				{
					int x1 = curRow[0]->getLocation().getX();
					int x2 = firstWidget->getLocation().getX();
					int diff = x2 - x1;

					for(size_t i = 0; i < curRow.size(); ++i)
					{
						curRow[i]->setLocation(
							curRow[i]->getLocation().getX() + diff,
							curRow[i]->getLocation().getY());
					}
				}

				else
				{
					int x1 = curRow[0]->getLocation().getX();
					int x2 = curRow.back()->getLocation().getX() +
						curRow.back()->getWidth();

					int w = x2 - x1;
					int centerOffset = (getInnerWidth() - w) / 2;

					for(size_t i = 0; i < curRow.size(); ++i)
					{
						curRow[i]->setLocation(
							curRow[i]->getLocation().getX() + centerOffset,
							curRow[i]->getLocation().getY());
					}
				}
			}
	}

	void FlowLayout::setHorizontalSpacing( int spacing )
	{
		horizontalSpacing = spacing;
		updateLayout();
	}

	void FlowLayout::setVerticalSpacing( int spacing )
	{
		verticalSpacing = spacing;
		updateLayout();
	}

	int FlowLayout::getHorizontalSpacing() const
	{
		return horizontalSpacing;
	}

	int FlowLayout::getVerticalSpacing() const
	{
		return verticalSpacing;
	}

	void FlowLayout::setLeftToRight( bool orientation )
	{
		leftToRight = orientation;
		updateLayout();
	}

	void FlowLayout::setTopToBottom( bool orientation )
	{
		topToBottom = orientation;
		updateLayout();
	}

	void FlowLayout::setSingleRow( bool single )
	{
		singleRow = single;
		updateLayout();
	}

	bool FlowLayout::isSingleRow() const
	{
		return singleRow;
	}

	bool FlowLayout::isLeftToRight() const
	{
		return leftToRight;
	}

	bool FlowLayout::isTopToBottom() const
	{
		return topToBottom;
	}

	bool FlowLayout::isHorizontallyCentered() const
	{
		return center;
	}

	void FlowLayout::setHorizontallyCentered( bool centered )
	{
		center = centered;
		updateLayout();
	}

	void FlowLayout::setAlignLastRow( bool align )
	{
		alignLastRow = align;
		updateLayout();
	}

	bool FlowLayout::isLastRowAligned() const
	{
		return alignLastRow;
	}


}
