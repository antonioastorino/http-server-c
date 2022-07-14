  function allowDrop(ev) {
    ev.preventDefault();
  }

  function drag(ev) {
    ev.dataTransfer.setData("text",ev.target.id);
  }

  function drop(ev) {
    ev.preventDefault();
    var data=ev.dataTransfer.getData("text");
    ev.target.appendChild(document.getElementById(data));
  }
       
  function yourFunction () {
     let div2 = document.getElementById("div0");
     for (let i = 0; i < 3; i++) {
        const newDiv = document.createElement("div");
        newDiv.setAttribute("id", `test_id_${i}`);
        const newContent = document.createTextNode("hi there and greetings!");
        newDiv.appendChild(newContent);
        document.body.insertBefore(newDiv, div2);
     }
  }
  window.onload = function() {
    yourFunction();
  };
 
