import QtQuick 2.0
import Material.Extras 0.1

QtObject {
   id: manifestorLoader

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

   function _url(date, loadNum) {
       return 'http://127.0.0.1:8080/cgi-bin/ajax2.cgi?query=' +
              'loadDetails&date=' + _jsonDate(date) + '&load=' + loadNum;
   }

   function _jsonDate(date) {
       return date.toISOString().substring(0,10);
   }

   function _incrementDate(date) {
       var newDate = new Date(date);
       newDate.setHours(24);
       return newDate;
   }

   function _startXnr(date, loadNum) {
       _request = new XMLHttpRequest();
       _request.onreadystatechange = _onreadystatechange;
       _request.open('GET', _url(
           date, loadNum), true);
       _request.send();
   }

   function _onreadystatechange() {
           if (_request.readyState === XMLHttpRequest.DONE) {
               if (_request.status === 200) {
                   var data = JSON.parse(_request.responseText);
                   if (data.people && data.people.length) {
                       data.date = _jsonDate(_currentDate);
                       data.loadNum = _currentLoad;
                       loaded(data);
                       _currentLoad +=1;
                       _currentEmpyLoads = 0;
                   } else {
                       _currentEmpyLoads += 1;
                       if (_currentEmpyLoads > _MAX_EMPTY_LOADS) {
                           _currentDate = _incrementDate(_currentDate)
                           progress = (_currentDate.getTime()
                                       - startDate.getTime())
                                   / (_today.getTime() - startDate.getTime())
                                   * 100;
                           _currentEmpyLoads = 0;
                           _currentLoad = 1;
                       }
                   }
               } else  {
                   working = false;
                   finished("error");
               }
               if (_currentDate.getTime() <= _today.getTime() && working) {
                   _startXnr(_currentDate, _currentLoad);
               } else {
                   working = false;
                   finished("done");
               }
           }
       }

   function process() {
       working = true;
       progress = 0;
       _today = new Date();
       _currentDate = startDate;
       _currentLoad = 1;
       _currentEmpyLoads = 0;
       _startXnr(_currentDate, 1);
   }
}
