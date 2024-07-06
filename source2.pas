program ExampleProgram(input, output);

label 10, 20;

var
  x, y, z: integer;
  a, b: real;
  flag: boolean;

procedure Swap(var m, n: integer);
var temp: integer;
begin
  temp := m;
  m := n;
  n := temp;
end;

function Sum(a, b: real): real;
begin
  Sum := a + b;
end;

begin
  read(x, y);
  z := 10;
  a := 5.5;
  b := 6.5;
  flag := true;

  if flag then
    write(1)
  else
    write(0);

  while x < y do
  begin
    x := x + 1;
    y := y - 1;
  end;

  if x = y then
  begin
    write('x and y are equal');
    goto 10;
  end;

  10: write('This is label 10');
  Swap(x, z);
  write('After swap, x = ', x, ' and z = ', z);

  b := Sum(a, b);
  write('Sum of a and b = ', b);

  readln;
  writeln('End of program');
end.
