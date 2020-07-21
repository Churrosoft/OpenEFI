let a = [195, 5, 0, 0];
const convertToInt = arr =>
  (arr[arr.length - 4] << 24) |
  (arr[arr.length - 3] << 16) |
  (arr[arr.length - 2] << 8) |
  arr[arr.length - 1];

let temp = convertToInt(a.slice(0, 4).reverse());
console.log(temp);
