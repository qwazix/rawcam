#ifndef EXAMPLE_OVERLAY_WIDGET_H
#define EXAMPLE_OVERLAY_WIDGET_H

#include <QMouseEvent>
#include "OverlayWidget.h"

class ExampleOverlayWidget : public OverlayWidget {
    Q_OBJECT;
	public:
		ExampleOverlayWidget(QWidget *parent = NULL);
	protected:
		void drawUI();
		bool inputHandler(QMouseEvent *);

signals:
    void focus(int, int);
    void shutter();

};


#endif
