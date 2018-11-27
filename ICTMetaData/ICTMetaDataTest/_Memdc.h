#ifndef _MEMDC_H_
#define _MEMDC_H_
/// @file _Memdc.h
/// @date 2014-05-07
/// @author Seungwon Lee
/// @brief memory DC
//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-2002, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support. - KR
//
//           11/3/99 Fixed most common complaint. Added
//                   background color fill. - KR
//
//           11/3/99 Added support for mapping modes other than
//                   MM_TEXT as suggested by Lee Sang Hun. - KR
//
//           02/11/02 Added support for CScrollView as supplied
//                    by Gary Kirkham. - KR
//
// This class implements a memory Device Context which allows
// flicker free drawing.
/** 
	@brief Memory DC Class
	@date 2014-05-01
*/
class _CMemDC : public CDC {
private:	
	CBitmap		m_bitmap;		// Offscreen bitmap
	CBitmap*	m_oldBitmap;	// bitmap originally found in CMemDC
	CDC*		m_pDC;			// Saves CDC passed in constructor
	CRect		m_rect;			// Rectangle of drawing area.
	CRect		m_Irect;	
	BOOL		m_bMemDC;		// TRUE if CDC really is a Memory DC.
public:
	
	_CMemDC(CDC* pDC, const CRect* pRect = NULL,const CRect* m_pIrect = NULL) : CDC()
	{
		ASSERT(pDC != NULL); 

		// Some initialization
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the rectangle to draw
		if (pRect == NULL) {
			pDC->GetClipBox(&m_rect);

		} else {
			m_rect = *pRect;
		}
		if (m_pIrect == NULL) {
			m_Irect=m_rect;

		} else {
			m_Irect = *m_pIrect;
		}

		if (m_bMemDC) {
			// Create a Memory DC
			CreateCompatibleDC(pDC);
			pDC->LPtoDP(&m_rect);
			pDC->LPtoDP(&m_Irect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_Irect.Width(), m_Irect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);

			SetMapMode(pDC->GetMapMode());

			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_Irect.left, m_Irect.top);
			SetWindowOrg(m_rect.left, m_rect.top);
		} else {
			// Make a copy of the relevent parts of the current DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		// Fill background 
		FillSolidRect(m_Irect, pDC->GetBkColor());
	}
	
	~_CMemDC()	
	{		
		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->SetStretchBltMode(COLORONCOLOR);  
			m_pDC->StretchBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_Irect.left,m_Irect.top,m_Irect.Width(),m_Irect.Height(), SRCCOPY);
			/*m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);		*/	
			
			//Swap back the original bitmap.
			SelectObject(m_oldBitmap);		
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.			
			m_hDC = m_hAttribDC = NULL;
		}	
	}
	
	// Allow usage as a pointer	
	_CMemDC* operator->() 
	{
		return this;
	}	

	// Allow usage as a pointer	
	operator _CMemDC*() 
	{
		return this;
	}
};

#endif