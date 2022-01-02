const R = 3;
const C = 3;
const N = R * C;

const TOTAL_WIDTH = 650;
const MARGIN = 20;
const CELL_WIDTH = (TOTAL_WIDTH - 2 * MARGIN) / N;
const FONT_SIZE = CELL_WIDTH / 3.5;

const COLORS = ["#e9e9ff", "#e9ffe9", "#ffe9e9", "#ffffe9", "#ffe9ff", "#e9ffff"];

const cages = [
    {cells: [8, ], sum: 1},
    {cells: [42, ], sum: 6},
    {cells: [65, ], sum: 8},
    {cells: [72, ], sum: 6},
    {cells: [78, ], sum: 8},
    {cells: [48, 57, ], sum: 7},
    {cells: [53, 62, ], sum: 13},
    {cells: [41, 50, ], sum: 6},
    {cells: [60, 61, ], sum: 10},
    {cells: [0, 1, 9, 18, ], sum: 16},
    {cells: [66, 75, 76, ], sum: 11},
    {cells: [14, 15, 23, ], sum: 17},
    {cells: [2, 11, 12, ], sum: 17},
    {cells: [5, 6, ], sum: 12},
    {cells: [54, 55, 63, ], sum: 9},
    {cells: [24, 31, 32, 33, ], sum: 25},
    {cells: [43, 44, 51, 52, ], sum: 26},
    {cells: [7, 16, 17, ], sum: 13},
    {cells: [25, 26, 34, 35, ], sum: 16},
    {cells: [69, 70, 71, 79, 80, ], sum: 21},
    {cells: [3, 4, 13, 22, ], sum: 20},
    {cells: [39, 40, 49, ], sum: 13},
    {cells: [27, 28, 29, 36, 37, ], sum: 20},
    {cells: [64, 73, 74, ], sum: 17},
    {cells: [38, 45, 46, 47, 56, ], sum: 30},
    {cells: [10, 19, 20, 21, 30, ], sum: 27},
    {cells: [58, 59, 67, 68, 77, ], sum: 30},
];

var colorMap = {};
var lastUsedColor = 0;

for (var i = 0; i < cages.length; ++i) {
    var unallowedColors = new Set();
    for (var j = 0; j < cages[i].cells.length; ++j) {
        const cell = cages[i].cells[j];
        const adjacentCells = [cell + 1, cell + N, cell - 1, cell - N];
        for (var k = 0; k < 4; ++k) {
            if (adjacentCells[k] in colorMap) {
                unallowedColors.add(colorMap[adjacentCells[k]]);
            }
        }
    }
    var colorIndex = undefined;
    for (var j = lastUsedColor; j <= lastUsedColor + COLORS.length; ++j) {
        if (!unallowedColors.has(COLORS[j % COLORS.length])) {
            colorIndex = j % COLORS.length;
            break;
        }
    }
    var color = COLORS[colorIndex];
    lastUsedColor = colorIndex;
    for (var j = 0; j < cages[i].cells.length; ++j) {
        const cell = cages[i].cells[j];
        const x = (cell % N) * CELL_WIDTH + MARGIN;
        const y = Math.floor(cell / N) * CELL_WIDTH + MARGIN;
        var rectangle = new Rectangle(new Point(x, y), new Point(x + CELL_WIDTH, y + CELL_WIDTH));
        var path = new Path.Rectangle(rectangle);
        path.fillColor = color;
        colorMap[cell] = color;
        if (j == 0) {
            var text = new PointText(new Point(x + FONT_SIZE / 4, y + FONT_SIZE));
            text.content = cages[i].sum.toString();
            text.fillColor = "black";
            text.fontSize = FONT_SIZE;
        }
    }
}

for (var i = 0; i <= N; ++i) {
    var coord = MARGIN + i * CELL_WIDTH;
    var hor = new Path();
    hor.strokeColor = 'black';
    hor.strokeWidth = i % R == 0 ? 3 : 1;
    hor.add(new Point(MARGIN - 1.5, coord));
    hor.add(new Point(TOTAL_WIDTH - MARGIN + 1.5, coord));
    var ver = new Path();
    ver.strokeColor = 'black';
    ver.strokeWidth = i % C == 0 ? 3 : 1;
    ver.add(new Point(coord, MARGIN - 1.5));
    ver.add(new Point(coord, TOTAL_WIDTH - MARGIN + 1.5));
}

