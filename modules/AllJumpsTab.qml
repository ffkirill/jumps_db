import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQml 2.2
import Material 0.1

Tab {
    id: allJumpsTab
    title: "All Jumps"
    property var model: null
    property string filter: ""
    property date startDate
    property date endDate
    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.topMargin: Units.dp(8)
            Layout.leftMargin: Units.dp(8)
            Layout.rightMargin: Units.dp(8)
            Layout.fillWidth: true
            TextField {
                placeholderText: "Start date"
                onTextChanged: {
                    try {
                        allJumpsTab.startDate = text
                    } catch (e) {
                        console.log("bad start date")
                        allJumpsTab.startDate = new Date();

                    }
                }
            }
            TextField {
                placeholderText: "End date"
                onTextChanged: {
                    try {
                        allJumpsTab.endDate = text
                    } catch (e) {
                        console.log("bad end date")
                        allJumpsTab.endDate = new Date();
                    }
                }
            }
            Item {
                Layout.fillWidth: true

            }
            TextField {
                text: allJumpsTab.filter
                Layout.alignment: Qt.AlignRight
                placeholderText: "Search..."
                onTextChanged: {
                    allJumpsTab.filter = text
                }
            }
        }
        JumpsListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Units.dp(8)
            elevation: 1
            model: allJumpsTab.model
        }
    }
}
