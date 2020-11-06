#include "panel.hpp"

Panel::Panel(QWidget *parent): QWidget(parent) {


}

MockPanel::MockPanel(QWidget *parent): Panel(parent) {
    setMinimumSize(100, 100);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
