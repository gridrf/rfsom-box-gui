#ifndef BUTTONELEMENTUI_H
#define BUTTONELEMENTUI_H

#include <QWidget>
#include <scriptresult.h>

namespace Ui {
class buttonElement;
}

class ButtonElementUi : public QWidget
{
	Q_OBJECT
	QString description;
	QString iconPath;
	ScriptResult *script;

	quint16 iconSize;
	bool icon;
	bool iconRight;
	Ui::buttonElement *ui;

private:
	QString iconString();

public:
	explicit ButtonElementUi(QWidget *parent=0);
	~ButtonElementUi();

	QString getIconPath() const;
	void setIconPath(const QString& value);
	QString getDescription() const;
	void setDescription(const QString& value);
	bool getIconRight() const;
	void setIconRight(bool value);
	quint16 getIconSize() const;
	void setIconSize(const quint16& value);

	ScriptResult *getScript() const;
	void setScript(ScriptResult *value);


public slots:
	virtual void click();
};




#endif // BUTTONELEMENTUI_H
