#include "Widget.h"
#include "StyleLoader.h"
#include "ui_Widget.h"

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTranslator>

QTranslator translator;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setMouseMoveEnabled(this);
    initUI();
    initAnim();
    loadStyle();
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::MouseMove) {
        if (controls_.find(o) != controls_.end()) {
            e->ignore();
        }
    }
    return QWidget::eventFilter(o, e);
}

void Widget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    auto actChinese = menu.addAction("简体中文", this, [this] {
        if (!translator.load("FaceSlider_zh_CN.qm")) {
            QMessageBox::critical(this,
                                  tr("错误"),
                                  tr("翻译包加载失败！请确保翻译包(FaceSlider_zh_CN.qm)"
                                     "存在且与可执行文件位于同一目录"));
            return;
        }
        QApplication::installTranslator(&translator);
        chinese_ = true;
    });
    actChinese->setCheckable(true);
    actChinese->setChecked(chinese_);
    auto actEnglish = menu.addAction("English", this, [this] {
        if (!translator.load("FaceSlider_en_US.qm")) {
            QMessageBox::critical(
                this,
                "Error",
                "Failed to load translation file! Please ensure the translation "
                "file(FaceSlider_en_US.qm)"
                "exists and is located in the same directory as the executable file");
            return;
        }
        QApplication::installTranslator(&translator);
        chinese_ = false;
    });
    actEnglish->setCheckable(true);
    actEnglish->setChecked(!chinese_);
    menu.exec(QCursor::pos());
    e->accept();
}

void Widget::showEvent(QShowEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    ui->face->setFaceX(x_);
    ui->face->setFaceY(y_);
    ui->face->setHappiness(happiness_);
    ui->face->setDrep(drep_);
    e->accept();
}

void Widget::resizeEvent(QResizeEvent *e)
{
    ui->face->setFaceX(x_);
    ui->face->setFaceY(y_);
    ui->face->setHappiness(happiness_);
    ui->face->setDrep(drep_);
    e->accept();
}

void Widget::enterEvent(QEnterEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    startAnim(0);
    e->accept();
}

void Widget::leaveEvent(QEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    ui->face->setFaceX(0.5);
    ui->face->setFaceY(0.5);
    ui->face->setHappiness(0.9);
    startAnim(1);
    e->accept();
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    auto pos = e->position();

    x_ = pos.x() / width();
    y_ = pos.y() / height();
    ui->face->setFaceX(x_);
    ui->face->setFaceY(y_);

    // calc happiness
    happiness_ = calcHappiness(pos);
    qDebug() << "happiness:" << happiness_;
    ui->face->setHappiness(happiness_);
    e->accept();
}

void Widget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}

void Widget::setMouseMoveEnabled(QWidget *parent)
{
    parent->setMouseTracking(true);
    parent->installEventFilter(this);
    for (int i = 0; i < parent->children().count(); ++i) {
        auto w = dynamic_cast<QWidget *>(parent->children().at(i));
        if (!w)
            continue;
        controls_.insert(w);
        setMouseMoveEnabled(w);
    }
}

double Widget::calcHappiness(const QPointF &pos)
{
    auto happyCenter = ui->btnHappy->geometry().toRectF().center();
    happyCenter += ui->footer->pos();
    auto unhappyCenter = ui->btnUnhappy->geometry().toRectF().center();
    unhappyCenter += ui->footer->pos();
    auto d1 = pos - happyCenter;
    auto d2 = pos - unhappyCenter;
    auto distHappy = qSqrt(d1.x() * d1.x() + d1.y() * d1.y());
    auto distUnhappy = qSqrt(d2.x() * d2.x() + d2.y() * d2.y());
    return qPow(distUnhappy / (distHappy + distUnhappy), 0.75);
}

void Widget::initUI()
{
    setAutoFillBackground(true);
    auto pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Widget::initAnim()
{
    anim_drep_ = new QPropertyAnimation(this, "drep", this);
    anim_drep_->setDuration(300);
    anim_drep_->setEasingCurve(QEasingCurve::OutQuad);
}

void Widget::startAnim(double drep)
{
    if (anim_drep_->state() == QAbstractAnimation::Running)
        anim_drep_->stop();
    anim_drep_->setStartValue(drep_);
    anim_drep_->setEndValue(drep);
    anim_drep_->start();
}

void Widget::loadStyle()
{
    StyleLoader().append(":/styles/button.qss").load(this);
}

void Widget::setDrep(double drep)
{
    drep_ = drep;
    ui->face->setDrep(drep_);
}

double Widget::drep() const
{
    return drep_;
}

void Widget::on_btnHappy_clicked()
{
    btn_clicked_ ^= 1;
    if (btn_clicked_) { // 点击取消
        ui->btnHappy->setText(tr("返回"));
        ui->btnUnhappy->hide();
        ui->title->setText(tr("已取消"));
        ui->subtitle->setText(tr("感谢您继续使用本插件"));
        ui->face->setFaceX(0.5);
        ui->face->setFaceY(0.5);
        ui->face->setHappiness(1);
        ui->face->setDrep(0);
    } else { // 点击返回
        ui->btnHappy->setText(tr("取消"));
        ui->btnUnhappy->show();
        ui->title->setText(tr("你确定要卸载吗"));
        ui->subtitle->setText(tr("希望不要卸载"));
    }
}

void Widget::on_btnUnhappy_clicked()
{
    btn_clicked_ ^= 1;
    if (btn_clicked_) {
        ui->btnUnhappy->setText(tr("返回"));
        ui->btnHappy->hide();
        ui->title->setText(tr("已删除"));
        ui->subtitle->setText(tr("感谢您使用本插件"));
        ui->face->setFaceX(0.5);
        ui->face->setFaceY(0.5);
        ui->face->setHappiness(0.2);
        ui->face->setDrep(0);
    } else { // 点击返回
        ui->btnUnhappy->setText(tr("卸载"));
        ui->btnHappy->show();
        ui->title->setText(tr("你确定要卸载吗"));
        ui->subtitle->setText(tr("希望不要卸载"));
    }
}
