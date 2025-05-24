#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class QPropertyAnimation;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void changeEvent(QEvent *e) override;

private slots:
    void on_btnHappy_clicked();
    void on_btnUnhappy_clicked();

private:
    Ui::Widget *ui;

    // 设置所有组件mouseTracking为true且需要传递给父组件
    void setMouseMoveEnabled(QWidget *parent);
    double calcHappiness(const QPointF &pos);
    void initUI();
    void initAnim();
    void startAnim(double drep);
    void loadStyle();

    QSet<QObject *> controls_;
    bool btn_clicked_ = false;
    bool chinese_ = true;
    double x_ = 0.5;
    double y_ = 0.5;
    double happiness_ = 0.9;

    void setDrep(double drep);
    double drep() const;
    Q_PROPERTY(double drep READ drep WRITE setDrep)
    double drep_ = 1.0;
    QPropertyAnimation *anim_drep_;
};
#endif // WIDGET_H
