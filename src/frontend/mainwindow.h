
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTranslator>

namespace Frontend
{

class CustomStatusBar;
class SessionEditor;
class ReportWorkspace;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* pParent = nullptr, bool isRestore = true);
    virtual ~MainWindow();

    SessionEditor* sessionEditor();
    ReportWorkspace* reportWorkspace();

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

    // Ui
    SessionEditor* mpSessionEditor;
    ReportWorkspace* mpReportWorkspace;

    // Translations
    QTranslator mTranslatorApplication;
    QTranslator mTranslatorQt;
};

void logMessage(QtMsgType type, QMessageLogContext const& /*context*/, QString const& message);
}

#endif // MAINWINDOW_H
