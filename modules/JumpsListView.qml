import QtQuick 2.0
import QtQuick.Layouts 1.1
import Material 0.1
import Material.ListItems 0.1 as ListItem
import org.ffkirill.jumpsdb 1.0

View {
    property JumpsSqlModel model: null
    id: jumpsListView
    ListView {
        anchors.fill: parent
        model: jumpsListView.model
        delegate: ListItem.Subtitled {
            id: item
            text: (jumpsListView.model.indexInGroup(index) + 1) + ". " + person
            subText: purpose
            maximumLineCount: 2
        }
        section.property: "group_val"
        section.criteria: ViewSection.FullString
        section.delegate: ListItem.Subheader {
            text: section
        }
    }
}
