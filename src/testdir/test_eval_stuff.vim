" Tests for various eval things.

function s:foo() abort
  try
    return [] == 0
  catch
    return 1
  endtry
endfunction

func Test_catch_return_with_error()
  call assert_equal(1, s:foo())
endfunc

func Test_nocatch_restore_silent_emsg()
  silent! try
    throw 1
  catch
  endtry
  echoerr 'wrong'
  let c1 = nr2char(screenchar(&lines, 1))
  let c2 = nr2char(screenchar(&lines, 2))
  let c3 = nr2char(screenchar(&lines, 3))
  let c4 = nr2char(screenchar(&lines, 4))
  let c5 = nr2char(screenchar(&lines, 5))
  call assert_equal('wrong', c1 . c2 . c3 . c4 . c5)
endfunc

func Test_mkdir_p()
  call mkdir('Xmkdir/nested', 'p')
  call assert_true(isdirectory('Xmkdir/nested'))
  try
    " Trying to make existing directories doesn't error
    call mkdir('Xmkdir', 'p')
    call mkdir('Xmkdir/nested', 'p')
  catch /E739:/
    call assert_report('mkdir(..., "p") failed for an existing directory')
  endtry
  " 'p' doesn't suppress real errors
  call writefile([], 'Xfile')
  call assert_fails('call mkdir("Xfile", "p")', 'E739')
  call delete('Xfile')
  call delete('Xmkdir', 'rf')
endfunc
