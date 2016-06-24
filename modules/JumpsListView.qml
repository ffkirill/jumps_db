import QtQuick 2.7
import QtQuick.Controls 1.4
import org.ffkirill.jumpsdb 1.0

TableView {
    id: jumpsListView
    section.property: "group_val"
    section.criteria: ViewSection.FullString
    section.delegate: Text {
        verticalAlignment: Text.AlignVCenter
        height: 30
        text: section
    }

    TableViewColumn {
        role: "person"
        title: "person"
        width: 200
        delegate: Text {
            text: (jumpsListView.model.indexInGroup(styleData.row) + 1)
                  + ". " + styleData.value
        }
    }

    TableViewColumn {
        role: "purpose"
        title: "purpose"
        width: 180
    }

    TableViewColumn {
        role: "date"
        title: "date"
        width: 100
    }

    TableViewColumn {
        role: "load_num"
        title: "load_num"
        width: 100
    }

}
