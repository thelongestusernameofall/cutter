#include "Notepad.h"
#include "ui_Notepad.h"

#include "MainWindow.h"

#include "utils/MdHighlighter.h"
#include "utils/Highlighter.h"
#include "utils/SvgIconEngine.h"

#include <QPlainTextEdit>
#include <QFont>
#include <QDebug>
#include <QFontDialog>
#include <QTextCursor>
#include <QMenu>


Notepad::Notepad(MainWindow *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);

    ui->undoButton->setIcon(QIcon(new SvgIconEngine(QString(":/img/icons/undo.svg"), palette().buttonText().color())));
    ui->redoButton->setIcon(QIcon(new SvgIconEngine(QString(":/img/icons/redo.svg"), palette().buttonText().color())));

    // Radare core found in:
    this->main = main;

    highlighter = new MdHighlighter(ui->notepadTextEdit->document());
    isFirstTime = true;
    this->notesTextEdit = ui->notepadTextEdit;

    // Increase notes document inner margin
    QTextDocument *docu = this->notesTextEdit->document();
    docu->setDocumentMargin(10);

    // Context menu
    ui->notepadTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->notepadTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showNotepadContextMenu(const QPoint &)));
    connect(ui->notepadTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    connect(CutterCore::getInstance(), SIGNAL(notesChanged(const QString &)), this, SLOT(updateNotes(const QString &)));

    updateNotes(CutterCore::getInstance()->getNotes());
}

Notepad::~Notepad() {}

void Notepad::on_fontButton_clicked()
{
    bool ok = true;

    QFont font = QFontDialog::getFont(&ok, ui->notepadTextEdit->font(), this) ;
    if (ok)
    {
        // the user clicked OK and font is set to the font the user selected
        //ui->notepadTextEdit->setFont(font);
        //ui->previewTextEdit->setFont(font);
        this->setFonts(font);
    }
}

void Notepad::setFonts(QFont font)
{
    ui->notepadTextEdit->setFont(font);
}

void Notepad::on_boldButton_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText("**" + text + "**");
    }
    else
    {
        cursor.insertText("****");
    }
}

void Notepad::on_italicsButton_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText("*" + text + "*");
    }
    else
    {
        cursor.insertText("**");
    }
}

void Notepad::on_h1Button_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText("# " + text);
    }
    else
    {
        cursor.insertText("# ");
    }
}

void Notepad::on_h2Button_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText("## " + text);
    }
    else
    {
        cursor.insertText("## ");
    }
}

void Notepad::on_h3Button_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        QString text = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText("### " + text);
    }
    else
    {
        cursor.insertText("### ");
    }
}

void Notepad::on_undoButton_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    QTextDocument *doc = ui->notepadTextEdit->document();
    doc->undo();
}

void Notepad::on_redoButton_clicked()
{
    QTextCursor cursor = ui->notepadTextEdit->textCursor();
    QTextDocument *doc = ui->notepadTextEdit->document();
    doc->redo();
}

void Notepad::on_searchEdit_returnPressed()
{
    QString searchString = ui->searchEdit->text();
    QTextDocument *document = ui->notepadTextEdit->document();

    if (isFirstTime == false)
        document->undo();

    if (!searchString.isEmpty())
    {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd())
        {
            highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull())
            {
                highlightCursor.movePosition(QTextCursor::WordRight,
                                             QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();
        isFirstTime = false;
    }
}

void Notepad::on_searchEdit_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);

    QString searchString = ui->searchEdit->text();
    QTextDocument *document = ui->notepadTextEdit->document();

    if (isFirstTime == false)
        document->undo();

    if (!searchString.isEmpty())
    {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd())
        {
            highlightCursor = document->find(searchString, highlightCursor);

            if (!highlightCursor.isNull())
            {
                //highlightCursor.movePosition(QTextCursor::WordRight,
                //                       QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();
        isFirstTime = false;
    }
}

void Notepad::on_searchEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    QString searchString = ui->searchEdit->text();
    QTextDocument *document = ui->notepadTextEdit->document();

    if (isFirstTime == false)
        document->undo();

    if (!searchString.isEmpty())
    {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd())
        {
            highlightCursor = document->find(searchString, highlightCursor);

            if (!highlightCursor.isNull())
            {
                //highlightCursor.movePosition(QTextCursor::WordRight,
                //                       QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();
        isFirstTime = false;
    }
}

void Notepad::showNotepadContextMenu(const QPoint &pt)
{
    // Set Notepad popup menu
    QMenu *menu = ui->notepadTextEdit->createStandardContextMenu();
    QTextCursor cur = ui->notepadTextEdit->textCursor();
    QAction *first = menu->actions().at(0);

    if (!cur.hasSelection())
    {
        cur.select(QTextCursor::WordUnderCursor);
    }

    addr = cur.selectedText();

    static const int maxLength = 20;
    if(addr.length() > maxLength)
    {
        addr = addr.left(maxLength-1) + "\u2026";
    }

    ui->actionSeekToSelection->setText(tr("Seek to \"%1\"").arg(addr));

    menu->insertAction(first, ui->actionSeekToSelection);
    menu->insertSeparator(first);
    ui->notepadTextEdit->setContextMenuPolicy(Qt::DefaultContextMenu);
    menu->exec(ui->notepadTextEdit->mapToGlobal(pt));
    delete menu;
    ui->notepadTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
}

void Notepad::on_actionSeekToSelection_triggered()
{
    Core()->seek(addr);
}

void Notepad::updateNotes(const QString &notes)
{
    disconnect(ui->notepadTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    ui->notepadTextEdit->setPlainText(notes);
    connect(ui->notepadTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

void Notepad::textChanged()
{
    CutterCore *core = CutterCore::getInstance();
    disconnect(core, SIGNAL(notesChanged(const QString &)), this, SLOT(updateNotes(const QString &)));
    core->setNotes(ui->notepadTextEdit->toPlainText());
    connect(core, SIGNAL(notesChanged(const QString &)), this, SLOT(updateNotes(const QString &)));
}
