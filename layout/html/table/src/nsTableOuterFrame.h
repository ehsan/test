/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */
#ifndef nsTableOuterFrame_h__
#define nsTableOuterFrame_h__

#include "nscore.h"
#include "nsContainerFrame.h"

class nsTableFrame;
struct OuterTableReflowState;

/**
 * main frame for an nsTable content object, 
 * the nsTableOuterFrame contains 0 or one caption frame, and a nsTableFrame
 * psuedo-frame (referred to as the "inner frame').
 * <P> Unlike other frames that handle continuing across breaks, nsTableOuterFrame
 * has no notion of "unmapped" children.  All children (caption and inner table)
 * have frames created in Pass 1, so from the layout process' point of view, they
 * are always mapped
 *
 */
class nsTableOuterFrame : public nsContainerFrame
{
public:

  /** instantiate a new instance of nsTableFrame.
    * @param aInstancePtrResult  the new object is returned in this out-param
    * @param aContent            the table object to map
    * @param aParent             the parent of the new frame
    *
    * @return  NS_OK if the frame was properly allocated, otherwise an error code
    */
  static nsresult NewFrame(nsIFrame** aInstancePtrResult,
                           nsIContent* aContent,
                           nsIFrame*   aParent);

  /** @see nsIFrame::Paint */
  NS_IMETHOD Paint(nsIPresContext& aPresContext,
                   nsIRenderingContext& aRenderingContext,
                   const nsRect& aDirtyRect);

  /** outer tables are reflowed in two steps.
    * Step 1:, we lay out the caption and the inner table with
    * height and width set to NS_UNCONSTRAINEDSIZE.
    * This gives us absolute minimum and maximum widths for each component.
    * In the second step, we set the caption and the inner table to 
    * the width of the widest component, given the table's style, width constraints
    * and compatibility mode.<br>
    * Step 2: With the widths now known, we reflow the captions and table.<br>
    * NOTE: for breaking across pages, this method has to account for table content 
    *       that is not laid out linearly vis a vis the frames.  
    *       That is, content hierarchy and the frame hierarchy do not match.
    *
    * @see NeedsReflow
    * @see ResizeReflowCaptionsPass1
    * @see ResizeReflowTopCaptionsPass2
    * @see ResizeReflowBottomCaptionsPass2
    * @see PlaceChild
    * @see ReflowMappedChildren
    * @see PullUpChildren
    * @see ReflowChild
    * @see nsTableFrame::ResizeReflowPass1
    * @see nsTableFrame::ResizeReflowPass2
    * @see nsTableFrame::BalanceColumnWidths
    * @see nsIFrame::Reflow 
    */
  NS_IMETHOD Reflow(nsIPresContext*      aPresContext,
                    nsReflowMetrics&     aDesiredSize,
                    const nsReflowState& aReflowState,
                    nsReflowStatus&      aStatus);

  /** @see nsContainerFrame */
  NS_IMETHOD CreateContinuingFrame(nsIPresContext*  aPresContext,
                                   nsIFrame*        aParent,
                                   nsIStyleContext* aStyleContext,
                                   nsIFrame*&       aContinuingFrame);

protected:

  /** protected constructor 
    * @see NewFrame
    */
  nsTableOuterFrame(nsIContent* aContent, nsIFrame* aParentFrame);

  /** return PR_TRUE if the table needs to be reflowed.  
    * the outer table needs to be reflowed if the table content has changed,
    * or if the table style attributes or parent max height/width have
    * changed.
    */
  PRBool NeedsReflow(const nsSize& aMaxSize);

  /** create all child frames for this table */
  nsresult CreateChildFrames(nsIPresContext* aPresContext);

  /**
    * Reflow the caption and get its max element size
    */
  nsReflowStatus ReflowCaptionPass1(nsIPresContext* aPresContext,
                                    OuterTableReflowState& aState);

  void          PlaceChild( OuterTableReflowState& aState,
                            nsIFrame*          aKidFrame,
                            const nsRect&      aKidRect,
                            nsSize*            aMaxElementSize,
                            nsSize&            aKidMaxElementSize);

  /**
   * Reflow the frames we've already created
   *
   * @param   aPresContext presentation context to use
   * @param   aState current inline state
   * @return  true if we successfully reflowed all the mapped children and false
   *            otherwise, e.g. we pushed children to the next in flow
   */
  PRBool        ReflowMappedChildren(nsIPresContext*        aPresContext,
                                     OuterTableReflowState& aState,
                                     nsSize*                aMaxElementSize);

  /**
   * Try and pull-up frames from our next-in-flow
   *
   * @param   aPresContext presentation context to use
   * @param   aState current inline state
   * @return  true if we successfully pulled-up all the children and false
   *            otherwise, e.g. child didn't fit
   */
  PRBool        PullUpChildren(nsIPresContext*        aPresContext,
                               OuterTableReflowState& aState,
                               nsSize*                aMaxElementSize);

  virtual nsReflowStatus ReflowChild( nsIFrame*              aKidFrame,
                                      nsIPresContext*        aPresContext,
                                      nsReflowMetrics&       aDesiredSize,
                                      const nsReflowState&   aKidReflowState,
                                      nsSize*                aMaxElementSize,
                                      OuterTableReflowState& aState);

  /** overridden here to handle special caption-table relationship
    * @see nsContainerFrame::VerifyTree
    */
  NS_IMETHOD VerifyTree() const;

  /** overridden here to handle special caption-table relationship
    * @see nsContainerFrame::PrepareContinuingFrame
    */
  void PrepareContinuingFrame(nsIPresContext*    aPresContext,
                              nsIFrame*          aParent,
                              nsIStyleContext* aStyleContext,
                              nsTableOuterFrame* aContFrame);

  /**
   * Remove and delete aChild's next-in-flow(s). Updates the sibling and flow
   * pointers.
   *
   * Updates the child count and content offsets of all containers that are
   * affected
   *
   * Overloaded here because nsContainerFrame makes assumptions about pseudo-frames
   * that are not true for tables.
   *
   * @param   aChild child this child's next-in-flow
   * @return  PR_TRUE if successful and PR_FALSE otherwise
   */
  PRBool DeleteChildsNextInFlow(nsIFrame* aChild);

  /**
    * Create the inner table frame (nsTableFrame). Handles initial
    * creation as well as creation of continuing frames
    */
  nsresult CreateInnerTableFrame(nsIPresContext* aPresContext,
                                 nsTableFrame*&  aTableFrame);

  nsresult RecoverState(OuterTableReflowState& aState, nsIFrame* aKidFrame);
  nsresult IncrementalReflow(nsIPresContext* aPresContext,
                             OuterTableReflowState& aState,
                             nsReflowMetrics& aDesiredSize,
                             const nsReflowState& aReflowState,
                             nsReflowStatus& aStatus);
  nsresult AdjustSiblingsAfterReflow(nsIPresContext*        aPresContext,
                                     OuterTableReflowState& aState,
                                     nsIFrame*              aKidFrame,
                                     nscoord                aDeltaY);

private:
  /** used to keep track of this frame's children */
  nsTableFrame *mInnerTableFrame;
  nsIFrame* mCaptionFrame;

  /** used to track caption max element size */
  PRInt32 mMinCaptionWidth;

  /** used to cache reflow results so we can optimize out reflow in some circumstances */
  nsReflowMetrics mDesiredSize;
  nsSize mMaxElementSize;

};



#endif
