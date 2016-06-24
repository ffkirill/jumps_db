import QtQuick 2.0
import org.ffkirill.jumpsdb 1.0

QtObject {
    id: manifestorLoader
    property JumpsSqlModel model: null
    property bool working: false
    property int progress: 0
    property date startDate: new Date("2011-01-01")
    property date _today: new Date()
    property date _currentDate: new Date()
    property int _currentLoad: 1
    property int _currentEmpyLoads: 0
    property var _request: null
    property int _MAX_EMPTY_LOADS: 3

    signal loaded(var data)
    signal finished(var e)

    function getUrl(date, loadNum) {
        return 'http://192.168.1.3/service/ajax2.php?query='+
                'loadDetails&date='
                + formatJsonDate(date)
                + '&load=' + loadNum
    }

    function formatJsonDate(date) {
        return date.toISOString().substring(0, 10)
    }

    function getNextDate(date) {
        var newDate = new Date(date)
        newDate.setDate(newDate.getDate() + 1);
        return newDate
    }

    function updateProgress() {
        progress = (_currentDate.getTime() - startDate.getTime()) /
            (_today.getTime() - startDate.getTime()) * 100
    }

    function startRequest(date, loadNum) {
        _request = new XMLHttpRequest()
        _request.onreadystatechange = requestOnReadyStateChange
        _request.open('GET', getUrl(date, loadNum), true)
        _request.send()
    }

    function requestOnReadyStateChange() {
        var pattern = pattern = /\},\s+\]/
        if (_request.readyState === XMLHttpRequest.DONE) {
            if (_request.status === 200) {
                var data = JSON.parse(
                    _request.responseText.replace(pattern, "}]"))
                if (data.people && data.people.length) {
                    data.date = formatJsonDate(_currentDate)
                    data.loadNum = _currentLoad
                    loaded(data)
                    _currentLoad += 1
                    _currentEmpyLoads = 0
                } else {
                    _currentEmpyLoads += 1
                    if (_currentEmpyLoads > _MAX_EMPTY_LOADS) {
                        _currentDate = getNextDate(_currentDate)
                        console.log('increment');
                        console.log(formatJsonDate(_currentDate));
                        updateProgress()
                        _currentEmpyLoads = 0
                        _currentLoad = 1
                    }
                }
            } else {
                working = false
                finished("error")
            }
            if (_currentDate.getTime() <= getNextDate(_today.getTime()) && working) {
                console.log(formatJsonDate(_currentDate));
                startRequest(_currentDate, _currentLoad)
            } else {
                working = false
                finished("done")
            }
        }
    }

    function process() {
        var startDate = manifestorLoader.model.getLastDate();
        if (startDate) {
            startDate = new Date(startDate);
            startDate.setHours(0);
        }
        working = true
        progress = 0
        _today = new Date()
        _currentDate = startDate
        _currentLoad = 1
        _currentEmpyLoads = 0
        startRequest(_currentDate, 1)
    }

    onLoaded: {
        for (var i=0; i<data.people.length; ++i) {
            manifestorLoader.model.addRecord({
                "person": data.people[i].name,
                "card": data.people[i].cardID,
                "purpose": data.people[i].task,
                "date": data.date,
                "load_num": data.loadNum
           });
        }
        manifestorLoader.model.refresh();
    }

}
