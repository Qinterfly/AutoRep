#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

namespace Frontend
{

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    CustomTabWidget(QWidget* pParent = nullptr);
    virtual ~CustomTabWidget() = default;

    bool tabsRenameable() const;
    void setTabsRenamable(bool flag);

    int find(QWidget* pWidget);
    void removePage(int index);
    void removeAllPages();
    void moveTab(int iFrom, int iTo);

protected:
    bool eventFilter(QObject* pObject, QEvent* pEvent) override;

private:
    void renameTabDialog(int iTab);

private:
    bool mRenameEnabled;
};

}

#endif // CUSTOMTABWIDGET_H
