#ifndef XVIZ_QT_CHART_ELEMENT_HPP
#define XVIZ_QT_CHART_ELEMENT_HPP

#include "ui_element_factory.hpp"
#include "chart/chart_widget.hpp"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>

class ChartElement: public UIElement {
    Q_OBJECT
public:
    ChartElement() ;

    void buildWidget(const UIElementFactory &fac, const QDomElement &ele, QWidget *parent) override ;

    void getChannels(QVector<QByteArray> &channels) const override {
        channels.append(channels_) ;
    }

    void updateState(const xviz::msg::StateUpdate &) override ;

    QWidget *widget() const override { return widget_ ; }

private:

    ChartWidget *widget_ ;
    QVector<QByteArray> channels_ ;
};

#endif
