function allowDrop(ev) {
  ev.preventDefault();
}

function drag(ev) {
  ev.dataTransfer.setData("text", ev.target.id);
}

function drop(ev) {
  ev.preventDefault();
  var data = ev.dataTransfer.getData("text");
  ev.target.appendChild(document.getElementById(data));
}

function load() {
  const xhttp = new XMLHttpRequest();
  xhttp.onload = function () {
    document.getElementById("right-frame").innerText = this.responseText;
  };
  xhttp.open("GET", "/assets/sample.txt");
  xhttp.send();
}

function changeView(ev) {
  let frames = document.getElementsByClassName("supbage");
  frames.forEach((elem) => {
    elem.style.display = "block";
  });
  if (ev.target.id == "button-configure") {
    document.getElementById("iframe-configure").style.display = "block";
  }
}

function generateInput() {
  let input_div = document.getElementById("multiple-input");
  for (let i = 0; i < 4; i++) {
    const new_div = document.createElement("div");
    const new_label = document.createElement("label");
    const new_input = document.createElement("input");
    new_input.setAttribute("id", `input_${i}`);
    new_input.setAttribute("name", `input_${i}`);
    new_input.setAttribute("value", `value ${i}`);
    new_label.setAttribute("for", `input_${i}`);
    new_label.innerText = `Label ${i}: `;
    new_div.appendChild(new_label);
    new_div.appendChild(new_input);
    input_div.appendChild(new_div);
  }
}

window.onload = function () {
  generateInput();
};
