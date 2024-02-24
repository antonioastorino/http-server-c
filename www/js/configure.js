const NUMBER_OF_LINES = 8;

function generateInput() {
  let input_div = document.getElementById("multiple-input");
  for (let i = 0; i < NUMBER_OF_LINES; i++) {
    const new_div = document.createElement("div");
    const new_label = document.createElement("label");
    const new_input = document.createElement("input");
    new_input.setAttribute("id", `input_${i}`);
    new_input.setAttribute("name", `input_${i}`);
    new_input.setAttribute("value", "OUT");
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
