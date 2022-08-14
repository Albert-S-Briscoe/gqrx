#ifndef DOCKRDS_H
#define DOCKRDS_H
#include <QDockWidget>
#include <QSettings>

namespace Ui {
    class DockRDS;
}


class DockRDS : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockRDS(QWidget *parent = 0);
    ~DockRDS();

    enum page {
        PAGE_DISABLED = 0,
        PAGE_RDS = 1,
        PAGE_SIS = 2,
        PAGE_NUM = 3
    };

    void setCurrentPage(int index);
    int  currentPage() const;

public slots:
    void updateRDS(QString text, int type);
    void updateSIS(std::string text[6]);
    void updateID3(std::string text[4]);
    void showEnabled();
    void showDisabled();
    void setRDSmode(bool cmd);

private:
    void ClearTextFields();

signals:
    void rdsDecoderToggled(bool);
    void rdsPI(QString text);

private slots:
    void on_rdsCheckbox_toggled(bool checked);

private:
    Ui::DockRDS *ui;        /*! The Qt designer UI file. */
};

#endif // DOCKRDS_H
