import QtQuick 2.4
import org.ffkirill.jumpsdb 1.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

import "./modules"


ApplicationWindow {
    title: "Jumps Database"
    visible: true
    width: 700
    height: 600

    JumpsSqlModel {
        id: jumpsModel
        filter: allJumpsTab.filter
        startDate: allJumpsTab.startDate
        endDate: allJumpsTab.endDate
    }

    ArbitraryQueryModel {
        id: reportsModel
    }

    ManifestorLoader {
        id: manifestorLoader
        model: jumpsModel
    }

    Action {
        id: actionUpdate
        text: "Update from manifestor"
        onTriggered: {
           var startDate = jumpsModel.getLastDate();
           if (startDate)
               manifestorLoader.startDate = new Date(startDate);
           manifestorLoader.process();
        }
        enabled: !manifestorLoader.working
    }

    TabView {
        id: page
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        AllJumpsTab {
            id: allJumpsTab
            model: jumpsModel
            updateAction: actionUpdate
        }

        ReportsTab {
            id: arbitraryReportsTab
            model: reportsModel
            title: "Reports"
        }
    }

//    ProgressCircle {
//        id: determinateProgress
//        anchors {
//            right: parent.right
//            bottom: parent.bottom
//            margins: Units.dp(32)
//        }
//        width: Units.dp(64)
//        height: Units.dp(64)
//        indeterminate: false
//        minimumValue: 0
//        maximumValue: 100
//        value: manifestorLoader.progress
//        visible: manifestorLoader.working

//        Label {
//            anchors.centerIn: parent
//            text: Math.round(determinateProgress.value) + "%"
//        }
//    }

}
