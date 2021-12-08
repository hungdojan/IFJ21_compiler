require "ifj21"

function main()
    local a:integer = 5
    local b:number = 4.9
    local c:number = 5.1

    if a >= c then
        write("ano,")
    else
        write("ne,")
    end

	if a > b then
        write("ano,")
    else
        write("ne,")
    end

	if a > 4.9 then
        write("ano,")
    else
        write("ne,")
    end

end

main()
