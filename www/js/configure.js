var lines = [];
var connections = [];
const LINE_IN = 254;
const LINE_OUT = 255;
const NUMBER_OF_LINES = 8;
const v_space = 40;
const h_space = 20;
const left_line_length = 100;
const right_line_length = 200;
const top_margin = 10;
const left_line_left_margin = 10;
const right_line_left_margin =
  left_line_left_margin + left_line_length + h_space;
const vert_line_spacing = 10;

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

function attachAllOutputsToInputs() {
  // Set everything that is not a number to OUT
  for (let line_nr = 0; line_nr < NUMBER_OF_LINES; line_nr++) {
    let input_elem = document.getElementById("input_" + line_nr.toString());
    pointed_element = document.getElementById("input_" + input_elem.value);
    if (!pointed_element) {
      input_elem.value = "OUT";
    }
  }
  
  // Assign "IN" if connected
  for (let line_nr = 0; line_nr < NUMBER_OF_LINES; line_nr++) {
    let input_elem = document.getElementById("input_" + line_nr.toString());
    if (input_elem.value === line_nr.toString()) {
      input_elem.value = "OUT";
    }
    pointed_element = document.getElementById("input_" + input_elem.value);
    if (pointed_element) {
      if (pointed_element.value === "OUT" || pointed_element.value === "IN") {
        pointed_element.value = "IN";
      } else {
        // Prevent removing a connection
        alert(`Cannot connect out ${line_nr} to out ${input_elem.value}`);
        input_elem.value = "OUT";
      }
    }
  }
}

function labelTextUpdate(ev) {
  let value = ev.target.value.trim();
  ev.target.value = value;
  let id = ev.target.id;
  let number_id = id.split("_")[1].trim();
  // Current index
  // Look for all the input with value == id and replace set their value to OUT
  attachAllOutputsToInputs();
  updateCanvas();
}

function generateInput() {
  let input_div = document.getElementById("multiple-input");
  for (let i = 0; i < NUMBER_OF_LINES; i++) {
    const new_div = document.createElement("div");
    const new_label = document.createElement("label");
    const new_input = document.createElement("input");
    new_input.setAttribute("id", `input_${i}`);
    new_input.setAttribute("name", `input_${i}`);
    new_input.setAttribute("value", "OUT");
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
  connections = [];
  for (let line_nr = 0; line_nr < NUMBER_OF_LINES; line_nr++) {
    input_value = document.getElementById(`input_${line_nr}`).value;
    // Check that the end-point exists
    let next_vert_line_x_pos =
      (connections.length + 1) * vert_line_spacing + right_line_left_margin;
    if (document.getElementById("input_" + input_value)) {
      input_value_number = Number(input_value);
      connections.push(
        new Line(
          new Point(
            next_vert_line_x_pos,
            top_margin + input_value_number * v_space
          ),
          new Point(next_vert_line_x_pos, top_margin + line_nr * v_space)
        )
      );
    }
  }
  connections.forEach((elem) => {
    elem.draw(ctx);
  });
}

function initLines() {
  for (let line_nr = 0; line_nr < NUMBER_OF_LINES; line_nr++) {
    new_left_line = new Line(
      new Point(left_line_left_margin, top_margin + line_nr * v_space),
      new Point(
        left_line_left_margin + left_line_length,
        top_margin + line_nr * v_space
      )
    );
    lines.push(new_left_line);
    new_right_line = new Line(
      new Point(right_line_left_margin, top_margin + line_nr * v_space),
      new Point(
        right_line_left_margin + right_line_length,
        top_margin + line_nr * v_space
      )
    );
    lines.push(new_right_line);
  }
}

window.onload = function () {
  initLines();
  generateInput();
  initCanvas();
};
