#include <QBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <spdlog/spdlog.h>

#include "node_content_widget.h"
#include "node_node.h"

static bool NODECONTENTDEBUG = false;

QDMNodeContentWidget::QDMNodeContentWidget(std::weak_ptr<Node> node, QWidget *parent)
	: QWidget(parent)
{
	symbol = 0;
	m_node = node;
	initUI();
	if (NODECONTENTDEBUG)
	{
		spdlog::info("NodeContentWidget create sucess");
	}
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &QDMNodeContentWidget::timerCallback);
    timer->start(1000); // 每隔 1000ms（1s）触发一次 timeout 信号
}

QDMNodeContentWidget::~QDMNodeContentWidget()
{
	if (NODECONTENTDEBUG)
	{
		spdlog::info("NodeContentWidget delete sucess");
	}
}
void QDMNodeContentWidget::timerCallback()
{
    // std::shared_ptr<Node> shared_node = m_node.lock();
    // // spdlog::info(shared_node->variable);
    // if (shared_node && !symbol)
    // {
    //     symbol = 1;
    //     m_textEdit_variable->setText(QString::fromStdString(shared_node->variable));
    //     m_textEdit_condition->setText(QString::fromStdString(shared_node->condition));
    //     m_textEdit_action->setText(QString::fromStdString(shared_node->action));
    // }
}
void QDMNodeContentWidget::initUI()
{
	auto Layout = new QVBoxLayout(this);
	Layout->setContentsMargins(0, 0, 0, 0); // 设置边距
	m_textEdit_variable = new QDMTextEdit("foo", this);
	m_textEdit_variable->setFixedSize(130, 27); // 设置宽度为 200 像素，高度为 100 像素
	m_textEdit_condition = new QDMTextEdit("foo", this);
	m_textEdit_condition->setFixedSize(130, 27); // 设置宽度为 200 像素，高度为 100 像素
	m_textEdit_action = new QDMTextEdit("foo", this);
	m_textEdit_action->setFixedSize(130, 27); // 设置宽度为 200 像素，高度为 100 像素
	connect(m_textEdit_variable, &QDMTextEdit::textChanged, this, &QDMNodeContentWidget::variable_onTextChanged);
	connect(m_textEdit_condition, &QDMTextEdit::textChanged, this, &QDMNodeContentWidget::condition_onTextChanged);
	connect(m_textEdit_action, &QDMTextEdit::textChanged, this, &QDMNodeContentWidget::action_onTextChanged);
	Layout->addWidget(m_textEdit_variable);
	Layout->addWidget(m_textEdit_condition);
	Layout->addWidget(m_textEdit_action);
	setLayout(Layout);
    std::shared_ptr<Node> shared_node = m_node.lock();
    if (shared_node && !symbol)
    {
        m_textEdit_variable->setText(QString::fromStdString(shared_node->variable));
        m_textEdit_condition->setText(QString::fromStdString(shared_node->condition));
        m_textEdit_action->setText(QString::fromStdString(shared_node->action));
    }
}

void QDMNodeContentWidget::setEditingFlag(bool flag)
{
	m_editingFlag = flag;
}

const rapidjson::Document QDMNodeContentWidget::serialize()
{
	rapidjson::Document d; // 创建JSON对象
	d.SetObject();
	d.AddMember("undefind", "temp_Nothing", d.GetAllocator());
	return d;
}

void QDMNodeContentWidget::deserialize(const rapidjson::Value &value)
{
}

QDMTextEdit::QDMTextEdit(const QString &text, QWidget *parent)
	: QTextEdit(text, parent)
{
}
void QDMNodeContentWidget::variable_onTextChanged()
{
	QString updatedText = m_textEdit_variable->toPlainText();
	std::shared_ptr<Node> shared_node = m_node.lock();
	if (shared_node)
		shared_node->variable = updatedText.toStdString();
	spdlog::info(updatedText.toStdString());
	// 在这里处理获取的文本，可以将其保存到成员变量中，或者执行其他操作
}
void QDMNodeContentWidget::condition_onTextChanged()
{
	QString updatedText = m_textEdit_condition->toPlainText();
	std::shared_ptr<Node> shared_node = m_node.lock();
	if (shared_node)
		shared_node->condition = updatedText.toStdString();
	// spdlog::info(updatedText.toStdString());
	//  在这里处理获取的文本，可以将其保存到成员变量中，或者执行其他操作
}
void QDMNodeContentWidget::action_onTextChanged()
{
	QString updatedText = m_textEdit_action->toPlainText();
	std::shared_ptr<Node> shared_node = m_node.lock();
	if (shared_node)
		shared_node->action = updatedText.toStdString();
	// spdlog::info(updatedText.toStdString());
	//  在这里处理获取的文本，可以将其保存到成员变量中，或者执行其他操作
}
void QDMTextEdit::keyPressEvent(QKeyEvent *e)
{
	QTextEdit::keyPressEvent(e);
}

void QDMTextEdit::focusInEvent(QFocusEvent *e)
{
	if (NODECONTENTDEBUG)
	{
		spdlog::info("focus in");
	}
	auto x = dynamic_cast<QDMNodeContentWidget *>(parentWidget());
	if (x != nullptr)
	{
		x->setEditingFlag(true);
	}
	QTextEdit::focusInEvent(e);
}

void QDMTextEdit::focusOutEvent(QFocusEvent *e)
{
	if (NODECONTENTDEBUG)
	{
		spdlog::info("focus out");
	}
	auto x = dynamic_cast<QDMNodeContentWidget *>(parentWidget());
	if (x != nullptr)
	{
		x->setEditingFlag(false);
	}
	QTextEdit::focusOutEvent(e);
}
