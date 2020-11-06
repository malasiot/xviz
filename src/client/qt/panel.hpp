#ifndef XVIZ_QT_PANEL_HPP
#define XVIZ_QT_PANEL_HPP

#include <QWidget>

#include <xviz/channel.hpp>
#include "session.pb.h"

class Panel : public QWidget {
    Q_OBJECT
public:
    Panel(QWidget *parent = nullptr);

    virtual void config(const std::vector<xviz::Channel> &channels) = 0 ;
    virtual void updateState(const xviz::msg::StateUpdate &) = 0;
};

class MockPanel : public Panel {
    Q_OBJECT
public:
    MockPanel(QWidget *parent = nullptr);

    void config(const std::vector<xviz::Channel> &channels) override {}
    void updateState(const xviz::msg::StateUpdate &) override {}
};

#endif
