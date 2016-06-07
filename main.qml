import QtQuick 2.4
import org.ffkirill.jumpsdb 1.0
import QtQuick.Layouts 1.1
import Material 0.2
import Material.ListItems 0.1 as ListItem
import "./modules"


ApplicationWindow {
    title: "Jumps Database"

    initialPage: page
    visible: true

    JumpsSqlModel {
        id: jumpsModel
        filter: allJumpsTab.filter
        startDate: allJumpsTab.startDate
        endDate: allJumpsTab.endDate
    }

    ManifestorLoader {
        id: manifestorLoader
        model: jumpsModel
    }

    TabbedPage {
        id: page
        title: "JumpRun Database"
        actions: [
            Action {
                name: "Update from manifestor"
                iconName: "file/cloud_download"
                onTriggered: {
                    manifestorLoader.process();
                }
                enabled: !manifestorLoader.working
            }
        ]

        AllJumpsTab {
            id: allJumpsTab
            model: jumpsModel
        }

        ReportsTab {
            id: arbitraryReportsTab
            title: "Reports"
        }
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
