program ExampleProgram;

var
  x, y: integer;
  z: real;
  flag: boolean;

procedure MyProcedure(a: integer; var b: real);
var
  temp: real;
begin
  temp := b;
  b := a + temp;
end;

function MyFunction(a: integer; b: real): boolean;
begin
  if a > b then
    MyFunction := true
  else
    MyFunction := false;
end;

begin
  x := 10.;
  y := 20;
  z := 15.5;
  flag := false;

  MyProcedure(x, z);
  
  if MyFunction(x, z) then
    flag := true
  else
    flag := false;
  
  while x < y do
  begin
    x := x + 1;
    if x = 15 then
      goto 100;
  end;

  100: write(x, y, z, flag);
end.
