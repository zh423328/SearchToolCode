---这个文件主要负责寻找想要的中文文字--
i = 0;

function DoFile(filepath,filepathto,filepathpathtxt)
	--读取文件--

	local BUFSIZE = 2^10 ;		--1k
	local file = assert(io.open(filepath,"r"));
	local fileto = io.open(filepathto,"w");
	local filetotxt = io.open(filepathpathtxt,"a+");

	if file then

		while true do

			----
			local str = file:read("*line");

			--末尾--
			if not str then break end

			str = str .. "\n";

			--local pat = '"%s-[^"]-[\128-\255]+[^"]-[^.][^h-H]%s-"';

		--字符查找和替换(-尽可能少的替换)
			local newstr = string.gsub(str,'"%s-[^"]-[\161-\254]+[^"]-%s-"',
					function(x)
					filetotxt:write(string.format("%d\t%s\n",i+1,string.sub(x,2,-2)));
					i=i+1;
					return string.format("GAMETXT->GetGameTxt(%d)",i)
					end );

			fileto:write(newstr);
		end
		
		file:close();
		fileto:close();
		filetotxt:close();
	end
end


--DoFile("C:\1\1.txt","C:\2\1.txt","C:\2\chinese.txt");

--[[function extractChinese(s)
    for m in string.gmatch(s,'"[\176-\254]+"') do
        print(m)
    end
end
ss = 'foo"中文"a"你好"foo'
extractChinese(ss)--]]