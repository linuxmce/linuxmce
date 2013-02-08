/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
==========================================================================

  while not copy and pasted, this file is essentially a rough clone of
  http://developer.qt.nokia.com/wiki/JavaScript_programmer#bca5b0c8a343c98d57f5861de654d018
  which is a tutorial on using listmodels with javascript, a direction this project is going.

 That being said, the purpose of this file is to be able to
 *Recieve data from c++
 *Place that data into the qml listmodel associated with 'AssignScreenShot.qml'
 *Act upon clicks to that list model and resultingly, communicate back to c++ which will then save the changes
 *Organize data into sections (left last as working prototype is more important)
  */



Array.prototype.attach = function(listmodel)
{
    this.__model=listmodel;
    this.flush();
}

// .flush() method to JavaScript Arrays: updates the attached ListModel to the contents of the Array
// automatically creates a role "value" for the ListModel if the array is flat
//^^ above notes i dont quite understand yet

Array.prototype.flush = function() {
    var i;
    while (this.__model.count > this.length) this.__model.remove(this.__model.count-1);
    for (i = 0; i < this.__model.count; i++) this.__model.set(i, typeof this[i] === 'object' ? this[i] : {value: this[i]});
    for (;i < this.length; i++) this.__model.append(typeof this[i] === 'object' ? this[i] : {value: this[i]});
    this.__model.sync();        // The model.sync() is for updates in WorkerScript
}

function prepareObject (undefinedObj)
{
    //this function is supposed to return an object we can populate the model with
}

var objectArray = []; //the working javascript array for this worker script

var init = function(list) { //qml is ready? i think this returns the loops to qml after initializing but not sure
var i; v;
    objectArray.attach(list) // this connects the listmodel passed in to the javascript array

    /*
      the next few lines from the example involved running a heavy computational algorthim then displaying the
      results. while we are investigating transport methods for data, i will note the message passing code

      for(i = 0, i < dce++objectcount, i++)
      {
      objectArray[i] = dceobject[i] //adds data
      objectarray.flush()           //updates listmode
      }
     */
}

//click handling on listmodel
var listClickedAt = function(index) {   // mouse click callback for the array item [i]
    var half, i;

    if (index === 0) return;
    half = objectArray.splice(index, objectArray.length-index);
    objectArray.unshift(half.shift());
    for (i = 0; i < half.length; i++) objectArray.push(half[i]);
    objectArray.flush();    // update the ListModel
}

var messages = {
    click: listClickedAt,
    init: init
    };

