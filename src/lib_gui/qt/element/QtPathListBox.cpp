#include "qt/element/QtPathListBox.h"

#include <QtGui/qevent.h>
#include <QLabel>
#include <QListWidget>
#include <QMimeData>

#include "qt/element/QtPathListBoxItem.h"

QtPathListBox::QtPathListBox(QWidget *parent, const QString& listName, SelectionPolicyType selectionPolicy)
	: QtListBox(parent, listName)
	, m_selectionPolicy(selectionPolicy)
{
	QLabel* dropInfoText = new QLabel("Drop Files & Folders");
	dropInfoText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	dropInfoText->setObjectName("dropInfo");
	dropInfoText->setAlignment(Qt::AlignRight);

	addWidgetToBar(dropInfoText);
}

QtPathListBox::SelectionPolicyType QtPathListBox::getSelectionPolicy() const
{
	return m_selectionPolicy;
}

const FilePath& QtPathListBox::getRelativeRootDirectory() const
{
	return m_relativeRootDirectory;
}

void QtPathListBox::setRelativeRootDirectory(const FilePath& dir)
{
	m_relativeRootDirectory = dir;
}

std::vector<FilePath> QtPathListBox::getPathsAsDisplayed() const
{
	std::vector<FilePath> paths;
	for (int i = 0; i < m_list->count(); ++i)
	{
		QtListBoxItem* widget = dynamic_cast<QtListBoxItem*>(m_list->itemWidget(m_list->item(i)));
		paths.push_back(FilePath(widget->getText().toStdWString()));
	}
	return paths;
}

std::vector<FilePath> QtPathListBox::getPathsAsAbsolute() const
{
	std::vector<FilePath> paths = getPathsAsDisplayed();
	for (FilePath& path: paths)
	{
		makeAbsolute(path);
	}
	return paths;
}

void QtPathListBox::setPaths(const std::vector<FilePath>& list, bool readOnly)
{
	clear();

	for (FilePath path : list)
	{
		makeRelative(path);
		QtListBoxItem* item = addListBoxItemWithText(QString::fromStdWString(path.wstr()));
		item->setReadOnly(readOnly);
	}
}

void QtPathListBox::makeAbsolute(FilePath& path) const
{
	if (!path.empty() && !path.isAbsolute() && !m_relativeRootDirectory.empty())
	{
		path = m_relativeRootDirectory.getConcatenated(path);
	}
}

void QtPathListBox::makeRelative(FilePath& path) const
{
	if (!m_relativeRootDirectory.empty())
	{
		const FilePath relPath = path.getRelativeTo(m_relativeRootDirectory);
		if (relPath.wstr().size() < path.wstr().size())
		{
			path = relPath;
		}
	}
}

void QtPathListBox::dropEvent(QDropEvent *event)
{
	foreach(QUrl url, event->mimeData()->urls())
	{
		FilePath path(url.toLocalFile().toStdWString());
		makeRelative(path);
		addListBoxItemWithText(QString::fromStdWString(path.wstr()));
	}
}

void QtPathListBox::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

QtListBoxItem* QtPathListBox::createListBoxItem(QListWidgetItem* item)
{
	return new QtPathListBoxItem(this, item);
}