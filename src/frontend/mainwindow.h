
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTranslator>

namespace Frontend
{

class CustomStatusBar;
class ViewManager;
class ProjectEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* pParent = nullptr, bool isRestore = true);
    virtual ~MainWindow();

public:
    static QString language;
    static CustomStatusBar* pStatusBar;

private:
    void initializeWindow();

    // Content
    void createContent();
    void createLanguageActions();
    void createHelpActions();
    void createConnections();

    // State
    void setModified(bool flag);
    void setTheme();
    void setLanguage(QString const& newLanguage);
    void applyLanguage();
    void restart();

    // Settings
    void saveSettings();
    void restoreSettings();

    // Dialogs
    void about();

    // Slots
    void processEdited();

private:
    QSettings mSettings;

    // Translations
    QTranslator mTranslatorApplication;
    QTranslator mTranslatorQt;
};

void logMessage(QtMsgType type, QMessageLogContext const& /*context*/, QString const& message);
}

#endif // MAINWINDOW_H
