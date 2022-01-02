const R = 3;
const C = 3;
const N = R * C;

const TOTAL_WIDTH = 650;
const MARGIN = 20;
const CELL_WIDTH = (TOTAL_WIDTH - 2 * MARGIN) / N;
const FONT_SIZE = CELL_WIDTH / 3.5;

const COLORS = [
    "#d0d0ff", "#d0ffd0", "#ffd0d0",
    "#ffffd0", "#ffd0ff", "#d0ffff",
    "#e0e0d0", "#e0d0e0", "#d0e0e0"];

const cages = [
    {cells: [72, ], sum: 3},
    {cells: [26, 35, ], sum: 13},
    {cells: [46, 55, ], sum: 14},
    {cells: [32, 41, 42, ], sum: 11},
    {cells: [71, 80, ], sum: 12},
    {cells: [2, 3, ], sum: 14},
    {cells: [69, 70, 78, 79, ], sum: 16},
    {cells: [77, 67, 68, 58, ], sum: 22},
    {cells: [21, 22, 31, ], sum: 20},
    {cells: [9, 18, 27, ], sum: 13},
    {cells: [6, 7, 8, ], sum: 11},
    {cells: [5, 14, 23, 24, ], sum: 19},
    {cells: [53, 59, 60, 61, 62, ], sum: 24},
    {cells: [0, 1, 10, 19, ], sum: 20},
    {cells: [33, 34, 43, 44, ], sum: 22},
    {cells: [66, 56, 57, ], sum: 19},
    {cells: [30, 39, 40, ], sum: 18},
    {cells: [73, 74, 75, 76, ], sum: 18},
    {cells: [65, 36, 45, 54, 63, 64, ], sum: 30},
    {cells: [15, 16, 17, 25, ], sum: 21},
    {cells: [28, 29, 37, 38, 47, ], sum: 17},
    {cells: [4, 11, 12, 13, 20, ], sum: 28},
    {cells: [48, 49, 50, 51, 52, ], sum: 20},
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
    for (var j = lastUsedColor + 1; j <= lastUsedColor + COLORS.length; ++j) {
        if (!unallowedColors.has(COLORS[j % COLORS.length])) {
            colorIndex = j % COLORS.length;
            break;
        }
    }
    var color = COLORS[colorIndex];
    lastUsedColor = colorIndex;
    var minCell = N * N;
    for (var j = 0; j < cages[i].cells.length; ++j) {
        const cell = cages[i].cells[j];
        const x = (cell % N) * CELL_WIDTH + MARGIN;
        const y = Math.floor(cell / N) * CELL_WIDTH + MARGIN;
        var rectangle = new Rectangle(new Point(x, y), new Point(x + CELL_WIDTH, y + CELL_WIDTH));
        var path = new Path.Rectangle(rectangle);
        path.fillColor = color;
        colorMap[cell] = color;
        if (cell < minCell) {
            minCell = cell;
        }
    }
    const x = (minCell % N) * CELL_WIDTH + MARGIN;
    const y = Math.floor(minCell / N) * CELL_WIDTH + MARGIN;
    var text = new PointText(new Point(x + FONT_SIZE / 4, y + FONT_SIZE));
    text.content = cages[i].sum.toString();
    text.fillColor = "black";
    text.fontSize = FONT_SIZE;
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
