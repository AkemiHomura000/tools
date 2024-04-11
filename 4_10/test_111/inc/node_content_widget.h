#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QTimer>
#include "Node_Serializable.h"

class Node;
class QDMTextEdit : public QTextEdit
{
public:
    QDMTextEdit(const QString &text, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
};
class QDMNodeContentWidget : public QWidget, public Serializable
{
    Q_OBJECT
public:
    QDMNodeContentWidget(std::weak_ptr<Node> node, QWidget *parent = nullptr);
    ~QDMNodeContentWidget();
    void setEditingFlag(bool flag);

    const rapidjson::Document serialize() override;
    void deserialize(const rapidjson::Value &value) override;

protected:
    void initUI();

private slots:
    void variable_onTextChanged();
    void condition_onTextChanged();
    void action_onTextChanged();
    void timerCallback();

private:
    // 【20240114】【class16】这两个变量暂时没用，只是为了删除QTextEdit的保护，后续看情况
    bool m_editingFlag = false;
    std::weak_ptr<Node> m_node;
    QTimer *timer;
    bool symbol;
    QDMTextEdit *m_textEdit_variable;  // 将 QDMTextEdit 作为成员变量
    QDMTextEdit *m_textEdit_condition; // 将 QDMTextEdit 作为成员变量
    QDMTextEdit *m_textEdit_action;    // 将 QDMTextEdit 作为成员变量
};
