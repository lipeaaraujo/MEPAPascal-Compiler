program Exemplo(input, output);
var
  n, i, c, soma, somaDou : LongInt;
begin
  readln(n);

  soma := 0;
  i := 0;
  while i < n do
  begin
    read(c);
    soma := soma + c;
    i := i + 1; 
  end;

  readln(somaDou);

  if somaDou = soma then
    writeln('Acertou')
  else
    writeln('Errou');

end.