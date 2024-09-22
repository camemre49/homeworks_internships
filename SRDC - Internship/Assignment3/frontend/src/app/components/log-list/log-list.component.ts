import {Component, Input, OnInit} from '@angular/core';
import {faSortDown, faSortUp, faSearch} from "@fortawesome/free-solid-svg-icons";
import {ApiService} from "../../services/api.service";
import {Log} from "../../../Log";

@Component({
  selector: 'app-log-list',
  templateUrl: './log-list.component.html',
  styleUrls: ['./log-list.component.css']
})
export class LogListComponent implements OnInit{
  @Input() findParameter: string;
  currentPage: number = 0;
  totalPages: number;
  sortByField: string = "date";
  sortDirection: boolean = false;
  logs: Log[];
  onSearch: boolean = false;

  protected readonly faSearch = faSearch;

  ngOnInit() {
    this.onSearch = false;
    this.apiService.getLog("", this.currentPage + 1, this.sortByField, this.sortDirection ? 1 : -1).subscribe((response) => {
      this.logs = response["logs"];
      this.totalPages = response["totalPages"]
    })
  }

  constructor(private apiService: ApiService) {
  }

  goPage(i: number) {
    this.currentPage = i;
    if(!this.onSearch){
      this.ngOnInit()
    }
    else{
      console.log(i);
      this.apiService.getLog(this.findParameter, this.currentPage + 1, this.sortByField, this.sortDirection ? 1 : -1).subscribe((response) => {
        this.logs = response["logs"];
        this.totalPages = response["totalPages"]
        this.onSearch = true;
      })
    }
  }

  goPrev(){
    if(this.currentPage == 0){
      return;
    }
    else{
      this.goPage(this.currentPage - 1)
    }
  }

  goNext(){
    if(this.currentPage == this.totalPages - 1){
      return;
    }
    else{
      this.goPage(this.currentPage + 1)
    }
  }

  search(){
    if(this.findParameter){
      this.apiService.getLog(this.findParameter, this.currentPage + 1, this.sortByField, this.sortDirection ? 1 : -1).subscribe((response) => {
        this.logs = response["logs"];
        this.totalPages = response["totalPages"]
        this.onSearch = true;
      })
    }
    else{
      this.goPage(0);
    }
  }

  sortBy(field){
    if(field === this.sortByField){
      console.log(this.sortByField)
      this.sortDirection = !this.sortDirection;
    }
    else{
      this.sortDirection = true;
    }
    this.sortByField = field;
    this.goPage(0);
  }
}
