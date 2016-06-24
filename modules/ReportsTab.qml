import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import org.ffkirill.jumpsdb 1.0

Tab {
    property ArbitraryQueryModel model: null
    property int historyOffset: 0
    id: reportsTab
    title: "Reports"
    Component
    {
        id: columnComponent
        TableViewColumn {
            width: 100
        }
    }
    ColumnLayout {
        anchors.fill: parent
        TextField {
            id: query
            Layout.fillWidth: true
            Layout.margins: 8
            placeholderText: "Query text"
            onEditingFinished: {
                reportsTab.model.setQuery(text);
                tableViewLoader.setSource("ReportsTableView.qml",
                                          {model: reportsTab.model});
            }
            Keys.onPressed: {
                var accepted = true;
                if (event.key == Qt.Key_Up) {
                    reportsTab.historyOffset += 1;
                } else if (event.key == Qt.Key_Down
                           && reportsTab.historyOffset > 0) {
                    reportsTab.historyOffset -= 1;
                } else {
                    accepted = false;
                }
                if (accepted) {
                    query.text = reportsModel.getFromHistory(
                                reportsTab.historyOffset);
                }
                event.accepted = accepted;
            }
        }
        Loader {
            id: tableViewLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            onLoaded: {
                var tableView = tableViewLoader.item;
                var i=0, columnCount=tableView.columnCount;
                var columns = reportsTab.model.getColumnNames();
                for (i=0; i<columns.length; ++i) {
                    var col = columnComponent.createObject(
                        tableView, {"role": columns[i], "title": columns[i]})
                    tableView.addColumn(col);
                }
            }
        }
    }
}
