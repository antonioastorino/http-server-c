function labelTextUpdate(ev) {
  let value = ev.target.value;
  let id = ev.target.id;
  let split_text = value.split(",").map((elem) => {
    return elem.trim();
  });
  console.log(split_text);
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
    new_input.addEventListener("change", labelTextUpdate);
    new_label.setAttribute("for", `input_${i}`);
    new_label.innerText = `Label ${i}: `;
    new_div.appendChild(new_label);
    new_div.appendChild(new_input);
    input_div.appendChild(new_div);
  }
}

function initCanvas() {
  let canvas = document.getElementById("canvas");
  const v_space = 100;
  const h_space = 20;
  const line_length = 100;
  let ctx = canvas.getContext("2d");
  for (let i = 0; i < 4; i++) {
    ctx.moveTo(0, 10 + i * v_space);
    ctx.lineTo(line_length, 10 + i * v_space);
    ctx.stroke();
    ctx.moveTo(line_length + h_space, 10 + i * v_space);
    ctx.lineTo(2 * line_length + h_space, 10 + i * v_space);
    ctx.stroke();
  }
}

window.onload = function () {
  generateInput();
  initCanvas();
};
