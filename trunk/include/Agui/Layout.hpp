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

#ifndef AGUI_LAYOUT_HPP
#define AGUI_LAYOUT_HPP
#include "Agui/Widget.hpp"

namespace agui 
{	/**
     * Abstract base class for all layouts.
	 *
	 * It will hook into each child's sizeChanged and 
	 * locationChanged and update the layout when this happens.
	 *
	 * Must implement:
	 *
	 * layoutChildren
     * @author Joshua Larouche
     * @since 0.1.0
     */
	class AGUI_CORE_DECLSPEC Layout :
		public Widget,
		public WidgetListener
	{
		bool isLayingOut;
		bool resizeToParent;
	protected:
	/**
	 * Will layout the children according to how the layout should.
	 * @since 0.1.0
     */
		virtual void layoutChildren() = 0;
	/**
	 * Empty paint event.
	 * @since 0.1.0
     */
		virtual void paintBackground(const PaintEvent &paintEvent);
	/**
	 * Empty paint event.
	 * @since 0.1.0
     */
		virtual void paintComponent(const PaintEvent &paintEvent);
	/**
	 * Called when a child widget's location changes.
	 * @since 0.1.0
     */
		virtual void locationChanged(Widget *source, const Point &location);
	/**
	 * Called when a child widget's size changes.
	 * @since 0.1.0
     */
		virtual void sizeChanged(Widget* source, const Dimension &size);
	/**
	 * Will resize the layout to fit its parent's innerSize.
	 * @since 0.1.0
     */
		virtual void parentSizeChanged();
	public:
	/**
	 * Sets whether or not the layout will set its size to its parent's size when its parent's size changes.
     * @since 0.1.0
	 */
	void setResizeToParent(bool resize);
		/**
	 * @return True if the layout will set its size to its parent's innerSize when its parent's size changes.
     * @since 0.1.0
	 */
	bool isResizingToParent() const;
	/**
	 * This is what should be called to update the layout. 
	 * You should never call layoutChildren directly.
     */
		void updateLayout();
		virtual void add(Widget *widget);
		virtual void remove(Widget *widget);
		virtual void setSize(const Dimension &size);
		virtual void setSize(int width, int height);

	/**
	 * Default constructor.
     */
		Layout(void);
	/**
	 * Default destructor.
     */
		virtual ~Layout(void);
	};
}

#endif