#include "ExampleOverlayWidget.h"

ExampleOverlayWidget::ExampleOverlayWidget(QWidget *parent) : OverlayWidget(parent) {}

void ExampleOverlayWidget::drawUI()
{
//	for (int y=0; y<64; y++) {
//		for (int x=0; x<64; x++) {
//			// shutter button
//			*(cur + (y + 10)*854*2 + (x+780)*2 + 1) = 0x7f; // luma
//			*(cur + (y + 10)*854*2 + (x+780)*2) = 0xef; // chroma
//			// focus button
//			*(cur + (y + 10 + 64 + 10)*854*2 + (x+780)*2 + 1) = 0x7f; // luma
//			*(cur + (y + 10 + 64 + 10)*854*2 + (x+780)*2) = 0x1f; // chroma
//		}
//	}
}

bool ExampleOverlayWidget::inputHandler(QMouseEvent *e)
{
//	if (e->x() > 780 && e->y() < 74) {
//		//shutter press
//		emit shutter();
//		return true;
//	} else if(e->x() > 780 && e->y() < 148) {
//		//focus press
//		emit focus();
//		return true;
//	}
//	return false;
    return false;
}


