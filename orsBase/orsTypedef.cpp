#include "orsTypedef.h"

orsRect_i operator+ (const orsRect_i& rect1, const orsRect_i& rect2)
{
	orsRect_i rect;
	rect.m_xmin = ors_min( rect1.m_xmin, rect2.m_xmin );
	rect.m_xmax = ors_max( rect1.m_xmax, rect2.m_xmax );

	rect.m_ymin = ors_min( rect1.m_ymin, rect2.m_ymin );
	rect.m_ymax = ors_max( rect1.m_ymax, rect2.m_ymax );

	if( rect.width() <= 0 || rect.height() <= 0 )
		return orsRect_i();

	return rect;
}