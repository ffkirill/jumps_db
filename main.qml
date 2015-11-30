import QtQuick 2.4
import org.ffkirill.jumpsdb 1.0
import QtQuick.Layouts 1.1
import "./modules"
import Material 0.1
import Material.ListItems 0.1 as ListItem


ApplicationWindow {
    title: "Jumps Database"
    theme {
        primaryColor: Palette.colors["blue"]["500"]
        primaryDarkColor: Palette.colors["blue"]["700"]
        accentColor: Palette.colors["red"]["A200"]
        tabHighlightColor: "white"
    }

    initialPage: page
    visible: true

    ManifestorLoader {
        id: manifestorLoader
        onLoaded: {
            for (var i=0; i<data.people.length; ++i) {
                jumpsModel.addRecord({
                                         "person": data.people[i].name,
                                         "card": data.people[i].cardID,
                                         "purpose": data.people[i].task,
                                         "date": data.date,
                                         "load_num": data.loadNum
                                     });
            }
            jumpsModel.refresh();
        }
    }

    JumpsSqlModel {
        id: jumpsModel
        filter: filter.text
    }

    Page {
        id: page
        title: "Jumps"
        actions: [

            Action {
                name: "Update from manifestor"
                iconName: "file/cloud_download"
                onTriggered: {
                    var startDate = jumpsModel.getLastDate();
                    if (startDate)
                        manifestorLoader.startDate = new Date(startDate);
                    manifestorLoader.process();
                }
                enabled: !manifestorLoader.working
            }
        ]
        ColumnLayout {
            anchors.fill: parent
            TextField {
                id: filter
                Layout.topMargin: Units.dp(8)
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                placeholderText: "Search..."
            }
            View {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: Units.dp(8)
                elevation: 1

                ListView {
                    id: jumpsListView
                    anchors.fill: parent
                    model: jumpsModel
                    delegate: ListItem.Subtitled {
                        id: item
                        text: (jumpsListView.model.indexInGroup(index) + 1)
                              + ". " + person
                        subText: purpose
                        maximumLineCount: 2
                    }
                    section.property: "group_val"
                    section.criteria: ViewSection.FullString
                    section.delegate: ListItem.Subheader {
                        text: section
                    }
                }
            }}
    }

    ProgressCircle {
        id: determinateProgress
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Units.dp(32)
        }
        width: Units.dp(64)
        height: Units.dp(64)
        indeterminate: false
        minimumValue: 0
        maximumValue: 100
        value: manifestorLoader.progress
        visible: manifestorLoader.working

        Label {
            anchors.centerIn: parent
            text: Math.round(determinateProgress.value) + "%"
        }
    }

}
