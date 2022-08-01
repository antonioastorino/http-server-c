var lines = [];

class Point {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
}

class Line {
  constructor(start, end) {
    this.start = start;
    this.end = end;
  }
  draw(ctx) {
    ctx.moveTo(this.start.x, this.start.y);
    ctx.lineTo(this.end.x, this.end.y);
    ctx.stroke();
  }
}

function labelTextUpdate(ev) {
  let value = ev.target.value;
  let id = ev.target.id;
  let split_text = value.split(",").map((elem) => {
    return elem.trim();
  });
  console.log(split_text);
  updateCanvas();
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
  let ctx = canvas.getContext("2d");
  lines.forEach((elem) => {
    elem.draw(ctx);
  });
}

function updateCanvas() {
  let canvas = document.getElementById("canvas");
  let ctx = canvas.getContext("2d");
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  initCanvas();
}

function initLines(number_of_lines) {
  const v_space = 100;
  const h_space = 20;
  const left_line_length = 100;
  const right_line_length = 200;
  const top_margin = 10;
  const left_line_left_margin = 10;
  const right_line_left_margin = left_line_left_margin + left_line_length + h_space;
  for (let line_nr = 0; line_nr < number_of_lines; line_nr++) {
    new_left_line = new Line(
      new Point(left_line_left_margin, top_margin + line_nr * v_space),
      new Point(left_line_left_margin + left_line_length, top_margin + line_nr * v_space)
    );
    lines.push(new_left_line);
    new_right_line = new Line(
      new Point(right_line_left_margin, top_margin + line_nr * v_space),
      new Point(right_line_left_margin + right_line_length, top_margin + line_nr * v_space)
    );
    lines.push(new_right_line);
  }
}

window.onload = function () {
  initLines(4);
  generateInput();
  initCanvas();
};
