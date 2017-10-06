#include "appvideoplayer.h"
#include <QJsonObject>
#include <QCoreApplication>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>
#include "common.h"
#include <QTime>
#include <QPlainTextEdit>

AppVideoPlayer::AppVideoPlayer(QJsonValue params, QLayout *lay, //QPlainTextEdit* te,
			       QWidget *parent) : App(parent) , params(params), lay(lay), te(nullptr)
{
	proc=nullptr;
	post_cmd="";
	hangOnFinish=false;
	if(params.toObject().contains("post_cmd"))
	{
		post_cmd=params.toObject()["post_cmd"].toString();
	}
	if(params.toObject().contains("hang_on_finish"))
	{
		hangOnFinish=params.toObject()["hang_on_finish"].toBool();
	}
	cmd = params.toObject()["cmd"].toString();

	te=new QPlainTextEdit(parent);
	te->setPlainText("");
	lay->addWidget(te);
	te->setReadOnly(true);
	te->setFocusPolicy(Qt::NoFocus);
	te->installEventFilter(this);
	te->setStyleSheet("font-size:8px");
	exitRequested=false;
	scrollToBottom=true;
}

AppVideoPlayer::~AppVideoPlayer()
{

}

void AppVideoPlayer::buildUi()
{
	load();
}

void AppVideoPlayer::destroyUi()
{
	unload();
	if(te!=nullptr)
	{
		te->removeEventFilter(this);
		te->deleteLater();
		te=nullptr;
	}
}

bool AppVideoPlayer::eventFilter(QObject *watched, QEvent *event)
{
	if(watched==te && event->type()==QEvent::Wheel)
	{
		QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

		auto val = te->verticalScrollBar()->value();
		if (wheelEvent->delta()>0) {
			val--;
		} else {
			val++;
		}
		te->verticalScrollBar()->setValue(val);
		if(val!=te->verticalScrollBar()->maximum())
		{
			scrollToBottom=false;
		}
	}
	return false;
}

void AppVideoPlayer::unload()
{
	if (proc!=nullptr) {
		disconnect(proc,SIGNAL(finished(int)),this,SLOT(handleExitCode(int)));
		proc->write("q");
		proc->waitForFinished(1000);		
		proc->kill();
		proc->deleteLater();
		if(!post_cmd.isEmpty())
		{
			proc = new QProcess(this);
			proc->start("/bin/sh",QStringList() << "-c" <<  post_cmd);
			proc->waitForFinished(1000);
			proc->kill();
			proc->deleteLater();
		}
		proc=nullptr;
		exitRequested=true;
	}
}

void AppVideoPlayer::load()
{
	//unload();
	proc = new QProcess(this);
	proc->setWorkingDirectory(sharedResPath);
	proc->start("/bin/sh",QStringList() << "-c" <<  cmd);
	qDebug()<<proc->readAll();
	connect(proc,SIGNAL(finished(int)),this,SLOT(handleExitCode(int)));
	connect(proc,SIGNAL(readyReadStandardError()),this,SLOT(readStdErr()));
}

void AppVideoPlayer::readStdErr()
{
	if(proc)
	{
		auto errStr=proc->readAllStandardError();//.simplified();
		te->insertPlainText(errStr);
		if(scrollToBottom)
			te->verticalScrollBar()->setValue(te->verticalScrollBar()->maximum());
	}
}

void AppVideoPlayer::handleExitCode(int exitCode)
{
	if(exitCode!=0)
	{
		QString text="Errorcode "+QString::number(exitCode,16)+"\n";
		te->insertPlainText(text);
		auto errStr=proc->readAllStandardError();
		te->insertPlainText(errStr);
		setFocus();
	}
	else
	{
		qDebug()<<"App finished";

		if(!exitRequested && !hangOnFinish)
		{
			QKeyEvent *kev = new QKeyEvent( QEvent::KeyPress,Qt::Key_Left,Qt::NoModifier);
			QApplication::sendEvent(this, kev);
		}
	}


}
