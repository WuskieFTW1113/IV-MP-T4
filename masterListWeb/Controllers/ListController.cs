using Microsoft.AspNetCore.Mvc;
using System.IO.MemoryMappedFiles;
using System.Text;
using System.Threading.Tasks;

namespace masterListWeb.Controllers
{
    [ApiController]
    [Route("")]
    public class ListController : ControllerBase
    {

        [HttpGet]
        public async Task<dynamic> Get()
        {
            var mappedFile = MemoryMappedFile.OpenExisting("masterlistPipe");

            using (var view = mappedFile.CreateViewAccessor())
            {
                byte[] bytes = new byte[1000]; //The length here must match with masterList/pipeHandler.cpp @ MapViewOfFile
                view.ReadArray(0, bytes, 0, bytes.Length);
                return Encoding.UTF8.GetString(bytes).Trim('\0');
            }
        }
    }
}
